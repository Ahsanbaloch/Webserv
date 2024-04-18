
#include "BodyExtractor.h"
#include "RequestHandler.h"

BodyExtractor::BodyExtractor()
	: handler(*new RequestHandler())
{
}

BodyExtractor::BodyExtractor(RequestHandler& src)
	: handler(src)
{
	body_bytes_consumed = 0;
}

BodyExtractor::~BodyExtractor()
{
}

BodyExtractor::BodyExtractor(const BodyExtractor& src)
	: handler(src.handler)
{
}

BodyExtractor& BodyExtractor::operator=(const BodyExtractor& src)
{
	if (this != &src)
	{
		handler = src.handler;
	}
	return (*this);
}

//// GETTERS ////

std::string	BodyExtractor::getTempBodyFilepath() const
{
	return (temp_body_filepath);
}

bool	BodyExtractor::getExtractionStatus() const
{
	return (extraction_status);
}


////// METHODS //////

void	BodyExtractor::extractBody()
{
	if (handler.getUnchunkingStatus())
	{
		printf("Here\n");
		// may move to other location then --> temp_body
		temp_body_filepath = handler.getUnchunkedDataFile(); // we already have the file just need to set it
		extraction_status = 1;
	}
	else
	{
		if (temp_body_filepath.empty())
		{
			std::ostringstream num_conversion;
			g_num_temp_raw_body++;
			num_conversion << g_num_temp_raw_body;
			temp_body_filepath = "www/temp_body/temp" + num_conversion.str() + ".bin";
		}

		handler.buf_pos++;
		int to_write = std::min(handler.getBytesRead() - handler.buf_pos, handler.getHeaderInfo().getBodyLength());
		outfile.open(temp_body_filepath, std::ios::app | std::ios::binary);
		outfile.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), to_write);
		handler.buf_pos += to_write;
		body_bytes_consumed += to_write;
		if (body_bytes_consumed >= handler.getHeaderInfo().getBodyLength())
			extraction_status = 1;
		outfile.close();;
	}
}
