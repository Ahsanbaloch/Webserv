
#include "UploadPlain.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

UploadPlain::UploadPlain()
	: AUploadModule()
{
	body_bytes_consumed = 0;
}

UploadPlain::UploadPlain(RequestHandler& src)
	: AUploadModule(src)
{
	body_bytes_consumed = 0;
}

UploadPlain::~UploadPlain()
{
}

UploadPlain::UploadPlain(const UploadPlain& src)
	: AUploadModule(src)
{
	body_bytes_consumed = src.body_bytes_consumed;
}

UploadPlain& UploadPlain::operator=(const UploadPlain& src)
{
	if (this != &src)
	{
		AUploadModule::operator=(src);
		body_bytes_consumed = src.body_bytes_consumed;
	}
	return (*this);
}


/////////// METHODS ///////////

void	UploadPlain::uploadData()
{
	if (filepath_outfile.empty())
	{
		if (!handler.getHeaderInfo().getFilename().empty())
		{
			filepath_outfile = getUploadDir() + handler.getHeaderInfo().getFilename();
			if (access(filepath_outfile.c_str(), F_OK) == 0)
			{
				handler.setStatus(403);
				throw CustomException("Forbidden");
			}
		}
		else
		{
			std::ostringstream num_conversion;
			g_num_temp_files++;
			num_conversion << g_num_temp_files;
			filepath_outfile = getUploadDir() + "textfile" + num_conversion.str() + ".txt"; // add the upload location dir to path
		}
	}

	if (handler.getHeaderInfo().getTEStatus())
	{
		input.open(handler.getUnchunkedDataFile(), std::ios::ate);
		if (!input.is_open())
		{
			handler.setStatus(500); // or 403 or other code?
			throw CustomException("Internal Server Error");
		}
		std::streamsize file_size = input.tellg();
		input.seekg(0, std::ios::beg);
		outfile.open(filepath_outfile, std::ios::app);
		if (!outfile.is_open())
		{
			handler.setStatus(500); // or 403 or other code?
			throw CustomException("Internal Server Error");
		}

		char buffer[BUFFER_SIZE];

		while (file_size > 0)
		{
			int write_size = std::min(BUFFER_SIZE, static_cast<int>(file_size));
			input.read(buffer, write_size);
			outfile.write(buffer, input.gcount());
			file_size -= input.gcount();
		}
		body_read = 1;
		input.close();
		remove(handler.getUnchunkedDataFile().c_str()); // check if file was removed
		outfile.close();
	}
	else
	{
		handler.buf_pos++;
		int to_write = std::min(handler.getBytesRead() - handler.buf_pos, handler.getHeaderInfo().getBodyLength());
		outfile.open(filepath_outfile, std::ios::app);
		if (!outfile.is_open())
		{
			handler.setStatus(500); // or 403 or other code?
			throw CustomException("Internal Server Error");
		}
		outfile.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), to_write);
		handler.buf_pos += to_write;
		body_bytes_consumed += to_write;
		if (body_bytes_consumed >= handler.getHeaderInfo().getBodyLength())
			body_read = 1;
		outfile.close();
	}
}
