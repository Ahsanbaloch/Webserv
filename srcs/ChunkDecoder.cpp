
#include "ChunkDecoder.h"
# include "RequestHandler.h"


////////// CONSTRUCTORS & DESTRUCTORS //////////

ChunkDecoder::ChunkDecoder() 
	: handler(*new RequestHandler())
{
	chunk_length = 0;
	total_chunk_size = 0;
	trailer_exists = 0;
	body_unchunked = 0;
	te_state = body_start;

}

ChunkDecoder::ChunkDecoder(RequestHandler& src)
	: handler(src)
{
	chunk_length = 0;
	total_chunk_size = 0;
	trailer_exists = 0;
	body_unchunked = 0;
	te_state = body_start;
}

ChunkDecoder::~ChunkDecoder()
{
}

ChunkDecoder::ChunkDecoder(const ChunkDecoder& src)
	: handler(src.handler)
{
	chunk_length = src.chunk_length;
	total_chunk_size = src.total_chunk_size;
	temp_filename_unchunked = src.temp_filename_unchunked;
	trailer_exists = src.trailer_exists;
	body_unchunked = src.body_unchunked;
	te_state = body_start;
}

ChunkDecoder& ChunkDecoder::operator=(const ChunkDecoder& src)
{
	if (this != &src)
	{
		handler = src.handler;
		chunk_length = src.chunk_length;
		total_chunk_size = src.total_chunk_size;
		temp_filename_unchunked = src.temp_filename_unchunked;
		trailer_exists = src.trailer_exists;
		body_unchunked = src.body_unchunked;
		te_state = body_start;
	}
	return (*this);
}


////////// GETTERS //////////

std::string	ChunkDecoder::getUnchunkedDataFile() const
{
	return (temp_filename_unchunked);
}

int	ChunkDecoder::getTotalChunkSize() const
{
	return (total_chunk_size);
}

bool	ChunkDecoder::getDecodingStatus() const
{
	return (body_unchunked);
}


////////// HELPER METHODS //////////

void	ChunkDecoder::storeChunkedData()
{
	if (temp_filename_unchunked.empty())
		temp_filename_unchunked = createTmpFilePath();

	temp_unchunked.open(temp_filename_unchunked.c_str(), std::ios::app | std::ios::binary);
	int to_write = std::min(handler.getBytesRead() - handler.buf_pos, chunk_length);
	temp_unchunked.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), to_write);
	handler.buf_pos += to_write;
	chunk_length -= to_write;
	temp_unchunked.close();
}

void	ChunkDecoder::checkMaxBodySize()
{
	if (total_chunk_size > handler.getSelectedServer().bodySize)
	{
		handler.setStatus(413);
		if (!temp_filename_unchunked.empty())
			remove(temp_filename_unchunked.c_str());
		throw CustomException("Content Too Large");
	}
}


////////// MAIN METHOD //////////

void	ChunkDecoder::unchunkBody()
{
	while (!body_unchunked && handler.buf_pos++ < handler.getBytesRead())
	{
		unsigned char ch = handler.buf[handler.buf_pos];

		switch (te_state)
		{
			case body_start:
				if ((ch >= '0' && ch <= '9'))
				{
					chunk_length = ch - '0';
					te_state = chunk_size;
					break;
				}
				else if (ch >= 'a' && ch <= 'f')
				{
					chunk_length = ch - 'a' + 10;
					te_state = chunk_size;
					break;
				}
				else if (ch >= 'A' && ch <= 'F')
				{
					chunk_length = ch - 'A' + 10;
					te_state = chunk_size;
					break;
				}
				else if (ch == 0)
					break;
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when unchunking body");
				}

			case chunk_size:
				if (ch >= '0' && ch <= '9')
				{
					chunk_length = chunk_length * 16 + (ch - '0');
					break;
				}
				else if (ch >= 'a' && ch <= 'f')
				{
					chunk_length = chunk_length * 16 + (ch - 'a' + 10);
					break;
				}
				else if (ch >= 'A' && ch <= 'F')
				{
					chunk_length = chunk_length * 16 + (ch - 'A' + 10);
					break;
				}
				else if (chunk_length == 0)
				{
					if (ch == CR)
					{
						te_state = chunk_size_cr;
						break;
					}
					else if (ch == LF)
					{
						te_state = chunk_trailer;
						break;
					}
					else if (ch == ';')
					{
						te_state = chunk_extension;
						break;
					}
					else
					{
						handler.setStatus(400);
						throw CustomException("Bad request: detected when unchunking body");
					}
				}
				else if (ch == CR)
				{
					total_chunk_size += chunk_length;
					checkMaxBodySize();
					te_state = chunk_size_cr;
					break;
				}
				else if (ch == LF)
				{
					total_chunk_size += chunk_length;
					checkMaxBodySize();
					te_state = chunk_data;
					break;
				}
				else if (ch == ';')
				{
					total_chunk_size += chunk_length;
					checkMaxBodySize();
					te_state = chunk_extension;
					break;
				}
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when unchunking body");
				}
			
			case chunk_size_cr:
				if (ch == LF && chunk_length > 0)
				{
					te_state = chunk_data;
					break;
				}
				else if (ch == LF && chunk_length == 0)
				{
					te_state = chunk_trailer;
					break;
				}
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when unchunking body");
				}

			case chunk_extension:
				if (ch == CR)
				{
					te_state = chunk_size_cr;
					break;
				}
				else if (ch == LF && chunk_length > 0)
				{
					te_state = chunk_data;
					break;
				}
				else if (ch == LF && chunk_length == 0)
				{
					te_state = chunk_trailer;
					break;
				}
				else
					break;

			case chunk_data:
				storeChunkedData();
				ch = handler.buf[handler.buf_pos];
				if (ch == CR)
				{
					te_state = chunk_data_cr;
					break;
				}
				else if (ch == LF)
				{
					te_state = body_start;
					break;
				}
				break;

			case chunk_data_cr:
				if (ch == LF)
				{
					te_state = body_start;
					break;
				}
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when unchunking body");
				}
			
			case chunk_trailer:
				if (ch == CR)
				{
					te_state = chunk_trailer_cr;
					break;
				}
				else if (ch == LF)
				{
					te_state = body_end;
					break;
				}
				else
				{
					trailer_exists = 1;
					break;
				}

			case chunk_trailer_cr:
				if (ch == LF)
				{
					te_state = body_end;
					break;
				}
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when unchunking body");
				}

			case body_end_cr:
				if (ch == LF)
				{
					body_unchunked = 1;
					break;
				}
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when unchunking body");
				}

			case body_end:
				if (ch == CR)
				{
					te_state = body_end_cr;
					break;
				}
				else if (ch == LF || !trailer_exists)
				{
					body_unchunked = 1;
					break;
				}
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when unchunking body");
				}
		}
	}
}
