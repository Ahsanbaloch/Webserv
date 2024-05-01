
#include "UploadMultipart.h"

/////////// CONSTRUCTORS & DESTRUCTORS ///////////

UploadMultipart::UploadMultipart()
	: AUploadModule()
{
	if (handler.getLocationConfig().uploadDir.empty())
	{
		handler.setStatus(404);
		throw CustomException("Not found: upload directory");
	}
	identifyBoundary();
	meta_data_size = 0;
	file_data_size = 0;
	saved_file_data = 0;
	write_size = 0;
	mp_state = mp_start;
	content_dispo_state = begin;
	content_type_state = begin2;
}

UploadMultipart::UploadMultipart(RequestHandler& src)
	: AUploadModule(src)
{
	if (handler.getLocationConfig().uploadDir.empty())
	{
		handler.setStatus(404);
		throw CustomException("Not found: upload directory");
	}
	identifyBoundary();
	meta_data_size = 0;
	file_data_size = 0;
	saved_file_data = 0;
	write_size = 0;
	mp_state = mp_start;
	content_dispo_state = begin;
	content_type_state = begin2;
}

UploadMultipart::~UploadMultipart()
{
}

UploadMultipart::UploadMultipart(const UploadMultipart& src)
	: AUploadModule(src)
{
	temp_value = src.temp_value;
	temp_key = src.temp_key;
	boundary = src.boundary;
	content_disposition = src.content_disposition;
	multipart_content_type = src.multipart_content_type;
	meta_data_size = src.meta_data_size;
	file_data_size = src.file_data_size;
	saved_file_data = src.saved_file_data;
	write_size = src.write_size;
	mp_state = src.mp_state;
	content_dispo_state = src.content_dispo_state;
	content_type_state = src.content_type_state;
}

UploadMultipart& UploadMultipart::operator=(const UploadMultipart& src)
{
	if (this != &src)
	{
		AUploadModule::operator=(src);
		temp_value = src.temp_value;
		temp_key = src.temp_key;
		boundary = src.boundary;
		content_disposition = src.content_disposition;
		multipart_content_type = src.multipart_content_type;
		meta_data_size = src.meta_data_size;
		file_data_size = src.file_data_size;
		saved_file_data = src.saved_file_data;
		write_size = src.write_size;
		mp_state = src.mp_state;
		content_dispo_state = src.content_dispo_state;
		content_type_state = src.content_type_state;
	}
	return (*this);
}


/////////// HELPER METHODS ///////////

void	UploadMultipart::identifyBoundary()
{
	std::string value = handler.getHeaderInfo().getHeaderFields()["content-type"];

	std::size_t boundary_value_pos = value.find("=");
	if (boundary_value_pos != std::string::npos)
		boundary = value.substr(boundary_value_pos + 1);
	else
	{
		handler.setStatus(400);
		throw CustomException("Bad request: boundary not found in request header");
	}
}

char	UploadMultipart::advanceChar()
{
	char ch;

	if (handler.getHeaderInfo().getTEStatus())
	{
		input.read(&ch, 1);
		meta_data_size++;
	}
	else
	{
		handler.buf_pos++;
		ch = handler.buf[handler.buf_pos];
	}
	return (ch);
}

void	UploadMultipart::checkBoundaryID()
{
	char ch = advanceChar();

	for (std::string::iterator it = boundary.begin(); it != boundary.end(); it++)
	{
		if (handler.buf_pos == handler.getBytesRead() - 1)
			break;
		ch = advanceChar();
		if (*it != ch)
		{
			handler.setStatus(400);
			throw CustomException("Bad request: could not identify boundary in multipart/form-data");
		}
	}
	ch = advanceChar();
	checkCleanTermination(ch);
	mp_state = mp_content_dispo;
}

void	UploadMultipart::checkCleanTermination(char ch)
{
	if (ch == CR)
	{
		ch = advanceChar();
		if (ch != LF)
		{
			handler.setStatus(400);
			throw CustomException("Bad request: when parsing multipart/form-data");
		}
	}
	else if (ch != LF)
	{
		handler.setStatus(400);
		throw CustomException("Bad request: when parsing multipart/form-data");
	}
}


void	UploadMultipart::checkContentDispoChar(char ch)
{
	switch (content_dispo_state)
	{
	case begin:
		if (ch == ':')
			content_dispo_state = type;
		break;
	
	case type:
		if (ch == ';')
		{
			content_disposition.insert(std::pair<std::string, std::string>("type", temp_value));
			temp_value.erase();
			content_dispo_state = var_key;
			break;
		}
		else if (ch == SP)
			break;
		else
		{
			temp_value.append(1, ch);
			break;
		}

	case var_key:
		if (ch == '=')
		{
			content_dispo_state = var_value;
			break;
		}
		else if (ch == SP)
			break;
		else
		{
			temp_key.append(1, ch);
			break;
		}

	case var_value:
		if (ch == ';')
		{
			content_disposition.insert(std::pair<std::string, std::string>(temp_key, temp_value));
			temp_key.erase();
			temp_value.erase();
			content_dispo_state = var_key;
			break;
		}
		else if (ch == '"')
			break;
		else
		{
			temp_value.append(1, ch);
			break;
		}
	}
}

void	UploadMultipart::checkFileExistence()
{
	filepath_outfile = handler.getLocationConfig().uploadDir + "/" + content_disposition["filename"];
	if (access(filepath_outfile.c_str(), F_OK) == 0)
	{
		handler.setStatus(403);
		throw CustomException("Forbidden: file already exists");
	}
}

void	UploadMultipart::saveContentDispo(char ch)
{
	if (handler.getHeaderInfo().getTEStatus())
	{
		while (ch != CR && ch != LF)
		{
			checkContentDispoChar(ch);
			input.read(&ch, 1);
			meta_data_size++;
		}
	}
	else
	{
		while (handler.buf_pos < handler.getBytesRead() && (handler.buf[handler.buf_pos] != CR && handler.buf[handler.buf_pos] != LF))
		{
			unsigned char ch = handler.buf[handler.buf_pos];
			checkContentDispoChar(ch);
			handler.buf_pos++;
		}
		if (handler.buf_pos >= handler.getBytesRead())
			return ;
		ch = handler.buf[handler.buf_pos];
	}
	checkCleanTermination(ch);
	content_disposition.insert(std::pair<std::string, std::string>(temp_key, temp_value));
	temp_key.erase();
	temp_value.erase();
	checkFileExistence();
	mp_state = mp_content_type;
}

void	UploadMultipart::checkContentTypeChar(char ch)
{
	switch (content_type_state)
	{
		case begin2:
			if (ch == ':')
				content_type_state = type_name;
			break;
		
		case type_name:
			if (ch == SP)
				break;
			else
			{
				multipart_content_type.append(1, ch);
				break;
			}
	}
}

void	UploadMultipart::saveContentType(char ch)
{
	if (handler.getHeaderInfo().getTEStatus())
	{
		while (ch != CR && ch != LF)
		{
			checkContentTypeChar(ch);
			input.read(&ch, 1);
			meta_data_size++;
		}
	}
	else
	{
		while (handler.buf_pos < handler.getBytesRead() && (handler.buf[handler.buf_pos] != CR && handler.buf[handler.buf_pos] != LF))
		{
			unsigned char ch = handler.buf[handler.buf_pos];
			checkContentTypeChar(ch);
			handler.buf_pos++;
		}
		if (handler.buf_pos >= handler.getBytesRead())
			return ;
		ch = handler.buf[handler.buf_pos];
	}
	checkCleanTermination(ch);
	mp_state = mp_empty_line;
}

void	UploadMultipart::calcFileSize()
{
	if (handler.getChunkDecoder() != NULL)
		file_data_size = handler.getChunkDecoder()->getTotalChunkSize() - meta_data_size - boundary.size() - 8;
	else
	{
		meta_data_size = handler.buf_pos - handler.getHeaderInfo().getBodyBeginning();
		file_data_size = handler.getHeaderInfo().getBodyLength() - meta_data_size - boundary.size() - 8;
	}
}

void	UploadMultipart::storeFileData()
{
	if (handler.getRequestLength() < handler.getHeaderInfo().getBodyBeginning() + handler.getHeaderInfo().getBodyLength() - static_cast<int>(boundary.size()) - 4)
	{
		write_size = handler.getBytesRead() - handler.buf_pos;
		saved_file_data += write_size;
	}
	else
	{
		write_size = file_data_size - saved_file_data;
		mp_state = mp_boundary_end;
	}

	outfile.open(filepath_outfile.c_str(), std::ios::app | std::ios::binary);
	if (!outfile.is_open())
	{
		handler.setStatus(500);
		throw CustomException("Internal Server Error");
	}
	outfile.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), write_size);
	handler.buf_pos += write_size;
	outfile.close();
}

void	UploadMultipart::storeUnchunkedFileData()
{
	outfile.open(filepath_outfile.c_str(), std::ios::app | std::ios::binary);
	if (!outfile.is_open())
	{
		handler.setStatus(500);
		throw CustomException("Internal Server Error");
	}

	char buffer[BUFFER_SIZE];
	while (file_data_size > 0)
	{
		write_size = std::min(BUFFER_SIZE, static_cast<int>(file_data_size));
		input.read(buffer, write_size);
		outfile.write(buffer, input.gcount());
		file_data_size -= input.gcount();
	}
	outfile.close();
	mp_state = mp_boundary_end;
}

void	UploadMultipart::parseBody(char ch)
{
	switch (mp_state)
	{
		case mp_start:
			if (ch == '-')
				mp_state = mp_boundary_id;
			else
			{
				handler.setStatus(400);
				throw CustomException("Bad request: detected when parsing multipart/form-data body");
			}
			// fall through
		case mp_boundary_id:
			checkBoundaryID();
			break;
		
		case mp_content_dispo:
			saveContentDispo(ch);
			break;

		case mp_content_type:
			if (ch == CR)
				break;
			else if (ch == LF)
			{
				calcFileSize();
				mp_state = mp_content;
				break;
			}
			else if (ch == 'C')
			{
				saveContentType(ch);
				break;
			}
			else
			{
				handler.setStatus(400);
				throw CustomException("Bad request: could not identify content type in multipart/form-data");
			}

		case mp_empty_line:
			if (ch == CR)
				break;
			else if (ch == LF)
			{
				calcFileSize();
				mp_state = mp_content;
				if (!handler.getHeaderInfo().getTEStatus())
					break;
			}
			else
			{
				handler.setStatus(400);
				throw CustomException("Bad request: could not identify content type in multipart/form-data");
			}
			// fall through
		case mp_content:
			if (handler.getHeaderInfo().getTEStatus())
				storeUnchunkedFileData();
			else
				storeFileData();
			break;
			
		case mp_boundary_end:
			body_read = 1;
			body_parsing_done = 1;
			break;
	}
}


/////////// MAIN METHODS ///////////

void	UploadMultipart::uploadData()
{
	if (handler.getChunkDecoder() != NULL)
	{
		input.open(handler.getChunkDecoder()->getUnchunkedDataFile().c_str(), std::ios::binary);
		if (!input.is_open())
		{
			handler.setStatus(500);
			throw CustomException("Internal Server Error: could not open decoded chunked data file");
		}
		char ch;
		while (!body_parsing_done)
		{
			input.read(&ch, 1);
			meta_data_size++;
			parseBody(ch);
		}
		input.close();
		remove(handler.getChunkDecoder()->getUnchunkedDataFile().c_str());
	}
	else
	{
		while (!body_parsing_done && handler.buf_pos < handler.getBytesRead())
		{
			handler.buf_pos++;
			unsigned char ch = handler.buf[handler.buf_pos];
			parseBody(ch);
		}
	}
}
