
#include "PLAINBody.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

PLAINBody::PLAINBody()
	: ARequestBody()
{
	body_bytes_consumed = 0;
}

PLAINBody::PLAINBody(RequestHandler& src)
	: ARequestBody(src)
{
	body_bytes_consumed = 0;
}

PLAINBody::~PLAINBody()
{
}

PLAINBody::PLAINBody(const PLAINBody& src)
	: ARequestBody(src)
{
	body_bytes_consumed = src.body_bytes_consumed;
}

PLAINBody& PLAINBody::operator=(const PLAINBody& src)
{
	if (this != &src)
	{
		ARequestBody::operator=(src);
		body_bytes_consumed = src.body_bytes_consumed;
	}
	return (*this);
}

///////// GETTERS //////////

std::map<std::string, std::string>	URLENCODEDBody::getDatabase() const
{
	return (database);
}


/////////// METHODS ///////////

void	PLAINBody::readBody()
{
	if (handler.getHeaderInfo().getTEStatus())
	{
		unchunkBody();
		// identify filename
		// probably not necessary as unChunk already stores in correct format; just need to identify correct filename

		// if (handler.body_read)
		// {
		// 	input.open(filename, std::ios::ate);
		// 	std::streamsize file_size = input.tellg();
		// 	input.seekg(0, std::ios::beg);
		// 	temp.open("plain.txt", std::ios::app);
		// 	char buffer[BUFFER_SIZE];

		// 	while (file_size > 0)
		// 	{
		// 		int write_size = std::min(BUFFER_SIZE, static_cast<int>(file_size));
		// 		input.read(buffer, write_size);
		// 		temp.write(buffer, input.gcount());
		// 		file_size -= input.gcount();
		// 	}
		// 	input.close();
		// 	temp.close();
		// }
	}
	else
	{
		// identify filename // how to?
		handler.buf_pos++;
		int to_write = std::min(handler.getBytesRead() - handler.buf_pos, handler.getHeaderInfo().getBodyLength());
		outfile.open("plain.txt", std::ios::app);
		outfile.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), to_write);
		handler.buf_pos += to_write;
		body_bytes_consumed += to_write;
		if (body_bytes_consumed >= handler.getHeaderInfo().getBodyLength())
			body_read = 1;
		outfile.close();
	}
}
