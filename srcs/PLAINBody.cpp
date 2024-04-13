
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

		if (handler.body_read)
		{
			input.open(filename, std::ios::ate);
			std::streamsize file_size = input.tellg();
			input.seekg(0, std::ios::beg);
			temp.open("plain.txt", std::ios::app);
			char buffer[BUFFER_SIZE];

			while (file_size > 0)
			{
				int write_size = std::min(BUFFER_SIZE, static_cast<int>(file_size));
				input.read(buffer, write_size);
				temp.write(buffer, input.gcount());
				file_size -= input.gcount();
			}
			input.close();
			temp.close();
		}
	}
	else
	{
		// identify filename
		// open file and write directly into it from buffer;
		// 
		//;
	}
}
