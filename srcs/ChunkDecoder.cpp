
#include "ChunkDecoder.h"
# include "RequestHandler.h"


////////// CONSTRUCTORS & DESTRUCTORS //////////

ChunkDecoder::ChunkDecoder() 
	: handler(*new RequestHandler())
{
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

	temp_unchunked.open(temp_filename_unchunked, std::ios::app | std::ios::binary);
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
					handler.setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 1"); // other exception?
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
				else if (chunk_length == 0) // how to end if body is distributed over multiple requests?
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
						te_state = chunk_extension; // are there more seperators? // seperate state for last extension?
						break;
					}
					else
					{
						handler.setStatus(400); // what is the correct error code?
						throw CustomException("Bad request 2"); // other exception?
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
					handler.setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 3"); // other exception?
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
					handler.setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 4"); // other exception?
				}

			case chunk_extension: // skip over chunk_extension // could also be done in a for loop // limit max size of chunk extension -- vulnerabilities
			// A server ought to limit the total length of chunk extensions received in a request to an amount reasonable for the services provided, in the same way that it applies length limitations and timeouts for other parts of a message, and generate an appropriate 4xx (Client Error) response if that amount is exceeded
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

			case chunk_data: // Limit for chunk length?
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
				// else
				// {
				// 	handler.setStatus(400); // what is the correct error code?
				// 	throw CustomException("Bad request 5"); // other exception?
				// }

			case chunk_data_cr:
				if (ch == LF)
				{
					te_state = body_start;
					break;
				}
				else
				{
					printf("prev char: %i, %c\n", handler.buf[handler.buf_pos - 1], handler.buf[handler.buf_pos - 1]);
					printf("bad char: %i, %c\n", ch, ch);
					std::cout << "buf pos: " << handler.buf_pos << std::endl;
					handler.setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 5"); // other exception?
				}
			
			// is the existence of trailers indicated in the headers
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
				else // maybe skip trailer in a for loop? // limit size of trailer?
				{
					trailer_exists = 1; // create loop so that this is not set every time
					break;
				}
				// trailer fields can be useful for supplying message integrity checks, digital signatures, delivery metrics, or post-processing status information
				// probably can just discard this section --> how to identify end?

			case chunk_trailer_cr:
				if (ch == LF)
				{
					te_state = body_end;
					break;
				}
				else
				{
					handler.setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 6"); // other exception?
				}

			case body_end_cr:
				if (ch == LF)
				{
					// body_parsing_done = 1;
					// body_read = 1;
					body_unchunked = 1;
					break;
				}
				else
				{
					handler.setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 7"); // other exception?
				}

			case body_end:
				if (ch == CR)
				{
					te_state = body_end_cr;
					break;
				}
				else if (ch == LF || !trailer_exists)
				{
					// body_parsing_done = 1;
					// body_read = 1;
					body_unchunked = 1;
					break;
				}
				else
				{
					handler.setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 8"); // other exception?
				}
		}
	}
}
