
#include "MULTIPARTBody.h"

/////////// CONSTRUCTORS & DESTRUCTORS ///////////

MULTIPARTBody::MULTIPARTBody()
	: ARequestBody()
{
	identifyBoundary();
	meta_data_size = 0;
	file_data_size = 0;
	saved_file_data = 0;
	write_size = 0;
	mp_state = mp_start;
	content_dispo_state = begin;
	content_type_state = begin2;
}

MULTIPARTBody::MULTIPARTBody(RequestHandler& src)
	: ARequestBody(src)
{
	identifyBoundary();
	meta_data_size = 0;
	file_data_size = 0;
	saved_file_data = 0;
	write_size = 0;
	mp_state = mp_start;
	content_dispo_state = begin;
	content_type_state = begin2;
}

MULTIPARTBody::~MULTIPARTBody()
{
}

MULTIPARTBody::MULTIPARTBody(const MULTIPARTBody& src)
	: ARequestBody(src)
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

MULTIPARTBody& MULTIPARTBody::operator=(const MULTIPARTBody& src)
{
	if (this != &src)
	{
		ARequestBody::operator=(src);
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

void	MULTIPARTBody::identifyBoundary()
{
	std::string value = handler.getHeaderInfo().getHeaderFields()["content-type"];

	std::size_t boundary_value_pos = value.find("=");
	if (boundary_value_pos != std::string::npos)
		boundary = value.substr(boundary_value_pos + 1);
	else
	{
		handler.setStatus(400); // what is the correct error code?
		throw CustomException("Could not identify boundary"); // other exception?
	}
}

char	MULTIPARTBody::advanceChar()
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

void	MULTIPARTBody::checkBoundaryID()
{
	// why necessary?
	char ch = advanceChar();

	for (std::string::iterator it = boundary.begin(); it != boundary.end(); it++) // also check handler.buf_pos < handler.getBytesRead()
	{
		ch = advanceChar();
		if (*it != ch)
		{
			handler.setStatus(400); // what is the correct error code?
			throw CustomException("Issue with boundary ID 1"); // other exception?
		}
	}
	ch = advanceChar();
	checkCleanTermination(ch);
	mp_state = mp_content_dispo;
}

void	MULTIPARTBody::checkCleanTermination(char ch)
{
	if (ch == CR)
	{
		ch = advanceChar();
		if (ch != LF)
		{
			handler.setStatus(400); // what is the correct error code?
			throw CustomException("Bad request"); // other exception?
		}
	}
	else if (ch != LF)
	{
		handler.setStatus(400); // what is the correct error code?
		throw CustomException("Bad request"); // other exception?
	}
}


void	MULTIPARTBody::checkContentDispoChar(char ch)
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

void	MULTIPARTBody::checkFileExistence()
{
	std::string file_path = handler.getLocationConfig().root + content_disposition["filename"]; // add upload dir
	if (access(file_path.c_str(), F_OK) == -1)
	{
		handler.setStatus(403);
		throw CustomException("Forbidden");
	}
}

void	MULTIPARTBody::saveContentDispo(char ch)
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
	// checkFileExistence();
	mp_state = mp_content_type;
}

void	MULTIPARTBody::checkContentTypeChar(char ch)
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

void	MULTIPARTBody::saveContentType(char ch)
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

void	MULTIPARTBody::calcFileSize()
{
	if (handler.getHeaderInfo().getTEStatus())
	{
		file_data_size = total_chunk_size - meta_data_size - boundary.size() - 8;
		// meta_data_size = handler.buf_pos - handler.body_beginning;
		// may have to adjust the extra padding of 8 (2x CRLFCRLF) based on client
		// file_data_size = handler.body_length - meta_data_size - boundary.size() - 8;
		// mp_state = mp_content;
		printf("file size: %i\n", file_data_size);
	}
	else
	{
		meta_data_size = handler.buf_pos - handler.getHeaderInfo().getBodyBeginning();
		// may have to adjust the extra padding of 8 (2x CRLFCRLF) based on client
		file_data_size = handler.getHeaderInfo().getBodyLength() - meta_data_size - boundary.size() - 8;
	}
}

void	MULTIPARTBody::storeFileData()
{
	// may have to adjust the extra padding of 4 (CRLFCRLF) based on client
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

	outfile.open(content_disposition["filename"], std::ios::app | std::ios::binary);

	std::cout << "body_beginning: " << handler.getHeaderInfo().getBodyBeginning() << std::endl;
	std::cout << "total_read " << handler.getRequestLength() << std::endl;
	std::cout << "meta_data: " << meta_data_size << std::endl;
	std::cout << "file size: " << file_data_size << std::endl;
	std::cout << "Boundary size: " << boundary.size() << std::endl;
	std::cout << "write size: " << write_size << std::endl;

	// body_beginning = 340 
	// boundary = 50
	// total_read = 8192 / 11644
	// body_len = 11303
	// meta_data = 149
	// file_size = 11104
	// writes: 7702 + 3401
	// bytes_written: 7702 + 3394 = 11096

	outfile.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), write_size);
	handler.buf_pos += write_size; // +/- 1?

	// printf("handler pos: %i, %c\n", handler.buf[handler.buf_pos], handler.buf[handler.buf_pos]);
	// printf("handler pos - 1: %i, %c\n", handler.buf[handler.buf_pos - 1], handler.buf[handler.buf_pos - 1]);
	outfile.close();
}

void	MULTIPARTBody::storeUnchunkedFileData()
{
	outfile.open(content_disposition["filename"], std::ios::app | std::ios::binary);
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

void	MULTIPARTBody::parseBody(char ch)
{
	switch (mp_state)
	{
		case mp_start:
			if (ch == '-')
				mp_state = mp_boundary_id;
			else
			{
				handler.setStatus(400); // what is the correct error code?
				throw CustomException("Bad request 20"); // other exception?
			}

		case mp_boundary_id:
			checkBoundaryID();
			break;
		
		case mp_content_dispo:
			saveContentDispo(ch);
			for (std::map<std::string, std::string>::iterator it = content_disposition.begin(); it != content_disposition.end(); it++)
			{
				std::cout << "key: " << it->first << " value: " << it->second << std::endl;
			}
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
				std::cout << "content type: " << multipart_content_type << std::endl;
				break;
			}
			else
			{
				handler.setStatus(400); // what is the correct error code?
				throw CustomException("Issue with content type"); // other exception?
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
				handler.setStatus(400); // what is the correct error code?
				throw CustomException("Issue with content type"); // other exception?
			}

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

void	MULTIPARTBody::readBody()
{
	if (handler.getHeaderInfo().getTEStatus())
	{
		unchunkBody();
		if (body_read)
		{
			body_parsing_done = 0;
			input.open(filename, std::ios::binary);
			char ch;
			while (!body_parsing_done)
			{
				input.read(&ch, 1);
				meta_data_size++;
				parseBody(ch);
			}
			input.close();
			remove(filename.c_str()); // check if file was removed
		}
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
