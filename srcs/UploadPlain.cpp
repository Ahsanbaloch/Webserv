
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
	filename = handler.getHeaderInfo().getFilename(); // add the upload location dir to path
	if (filename.empty())
	{
		std::ostringstream num_conversion;
		g_num_temp_files++;
		num_conversion << g_num_temp_files;
		filename = "www/" + num_conversion.str() + ".txt"; // add the upload location dir to path
	}
	// check for file existence
	if (handler.getHeaderInfo().getTEStatus())
	{
		// unchunkBody();

		// if (handler.body_read)
		// {
			input.open(handler.getUnchunkedDataFile(), std::ios::ate);
			std::streamsize file_size = input.tellg();
			input.seekg(0, std::ios::beg);
			outfile.open(filename, std::ios::app);
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
			// remove(handler.getUnchunkedDataFile().c_str()); // check if file was removed
			outfile.close();
		// }
	}
	else
	{
		handler.buf_pos++;
		int to_write = std::min(handler.getBytesRead() - handler.buf_pos, handler.getHeaderInfo().getBodyLength());
		outfile.open(filename, std::ios::app);
		outfile.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), to_write);
		handler.buf_pos += to_write;
		body_bytes_consumed += to_write;
		if (body_bytes_consumed >= handler.getHeaderInfo().getBodyLength())
			body_read = 1;
		outfile.close();
	}
}
