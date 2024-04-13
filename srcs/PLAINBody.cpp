
#include "PLAINBody.h"

// PLAINBody::PLAINBody()
// {
// }

PLAINBody::PLAINBody(RequestHandler& src)
	: ARequestBody(src)
{
}

PLAINBody::~PLAINBody()
{
}

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
		int to_write = std::min(handler.getBytesRead() - handler.buf_pos, handler.body_length);
		temp.open("plain.txt", std::ios::app);
		temp.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), to_write);
		handler.buf_pos += to_write;
		handler.body_length -= to_write;
		printf("body length remaining: %i\n", handler.body_length);
		if (handler.body_length <= 0)
			handler.body_read = 1;
		temp.close();
	}
}
