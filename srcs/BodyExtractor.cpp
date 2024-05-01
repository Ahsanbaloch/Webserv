
#include "BodyExtractor.h"
#include "RequestHandler.h"


///////// CONSTRUCTORS & DESTRUCTORS ///////////

BodyExtractor::BodyExtractor()
	: handler(*new RequestHandler()), body_bytes_consumed(0), extraction_status(0)
{
}

BodyExtractor::BodyExtractor(RequestHandler& src)
	: handler(src), body_bytes_consumed(0), extraction_status(0)
{
}

BodyExtractor::~BodyExtractor()
{
}

BodyExtractor::BodyExtractor(const BodyExtractor& src)
	: handler(src.handler)
{
	temp_body_filepath = src.temp_body_filepath;
	body_bytes_consumed = src.body_bytes_consumed;
	extraction_status = src.extraction_status;
}

BodyExtractor& BodyExtractor::operator=(const BodyExtractor& src)
{
	if (this != &src)
	{
		handler = src.handler;
		temp_body_filepath = src.temp_body_filepath;
		body_bytes_consumed = src.body_bytes_consumed;
		extraction_status = src.extraction_status;
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
	if (handler.getChunkDecoder() != NULL)
	{
		temp_body_filepath = handler.getChunkDecoder()->getUnchunkedDataFile();
		extraction_status = 1;
	}
	else
	{
		if (temp_body_filepath.empty())
			temp_body_filepath = createTmpFilePath();

		handler.buf_pos++;
		int to_write = std::min(handler.getBytesRead() - handler.buf_pos, handler.getHeaderInfo().getBodyLength());
		outfile.open(temp_body_filepath.c_str(), std::ios::app | std::ios::binary);
		outfile.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), to_write);
		handler.buf_pos += to_write;
		body_bytes_consumed += to_write;
		if (body_bytes_consumed >= handler.getHeaderInfo().getBodyLength())
			extraction_status = 1;
		outfile.close();
	}
}
