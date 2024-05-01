
#include "UploadPlain.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

UploadPlain::UploadPlain()
	: AUploadModule()
{
	if (handler.getLocationConfig().uploadDir.empty())
	{
		handler.setStatus(404);
		throw CustomException("Not found: upload directory");
	}
	body_bytes_consumed = 0;
}

UploadPlain::UploadPlain(RequestHandler& src)
	: AUploadModule(src)
{
	if (handler.getLocationConfig().uploadDir.empty())
	{
		handler.setStatus(404);
		throw CustomException("Not found: upload directory");
	}
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


///////// HELPER METHODS ///////////

void	UploadPlain::checkFilepath()
{
	if (filepath_outfile.empty())
	{
		if (!handler.getHeaderInfo().getFilename().empty())
		{
			filepath_outfile = handler.getLocationConfig().uploadDir + "/" + handler.getHeaderInfo().getFilename();
			if (access(filepath_outfile.c_str(), F_OK) == 0)
			{
				handler.setStatus(403);
				throw CustomException("Forbidden: file already exists");
			}
		}
		else
		{
			while (1)
			{
				filepath_outfile = handler.getLocationConfig().uploadDir + "/" + genRandomFileName(10) + ".txt";
				if (access(filepath_outfile.c_str(), F_OK) != 0)
					break;
			}
		}
	}
}

void	UploadPlain::openFiles()
{
	input.open(handler.getChunkDecoder()->getUnchunkedDataFile().c_str(), std::ios::ate);
	if (!input.is_open())
	{
		handler.setStatus(500);
		throw CustomException("Internal Server Error: could not open file");
	}
	outfile.open(filepath_outfile.c_str(), std::ios::app);
	if (!outfile.is_open())
	{
		handler.setStatus(500);
		throw CustomException("Internal Server Error: could not open file");
	}
}


/////////// MAIN METHOD ///////////

void	UploadPlain::uploadData()
{
	checkFilepath();

	if (handler.getChunkDecoder() != NULL)
	{
		openFiles();
		std::streamsize file_size = input.tellg();
		input.seekg(0, std::ios::beg);
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
		remove(handler.getChunkDecoder()->getUnchunkedDataFile().c_str());
		outfile.close();
	}
	else
	{
		handler.buf_pos++;
		int to_write = std::min(handler.getBytesRead() - handler.buf_pos, handler.getHeaderInfo().getBodyLength());
		outfile.open(filepath_outfile.c_str(), std::ios::app);
		if (!outfile.is_open())
		{
			handler.setStatus(500);
			throw CustomException("Internal Server Error: could not open file");
		}
		outfile.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), to_write);
		handler.buf_pos += to_write;
		body_bytes_consumed += to_write;
		if (body_bytes_consumed >= handler.getHeaderInfo().getBodyLength())
			body_read = 1;
		outfile.close();
	}
}
