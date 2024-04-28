
#include "CGIResponse.h"

CGIResponse::CGIResponse(RequestHandler& src)
	: AResponse(src)
{
	cgi_he_complete = 0;
	cgi_resp_he_state = he_start;
}

CGIResponse::CGIResponse(/* args */)
	: AResponse()
{
}

CGIResponse::~CGIResponse()
{
}


std::string	CGIResponse::getTempBodyFilePath()
{
	return (temp_body_filepath);
}

void	CGIResponse::readCGIHeader()
{
	std::string	line;

	while(std::getline(body_file, line))
	{
		std::cout << "line: " << line << std::endl;
		if (line.find("Status") != std::string::npos)
		{
			std::string status;
			for (std::string::iterator it = line.begin(); it != line.end(); it++)
			{
				if (isdigit(*it))
					status += *it;
			}
			handler.setStatus(toInt(status));
			if (toInt(status) >= 400)
				throw CustomException("CGI Error");
		}
		else if (!line.empty())
			header_fields.append(line + "\r\n");
		else
			break;
		
	}
	file_position = body_file.tellg();
	file_pos_offset = file_position;
	body_size = body_size - file_position;
}

void	CGIResponse::readHeaderFields()
{
	unsigned char	ch;
	std::string		header_name = "";
	std::string		header_value = "";

	// what to do if there is a syntax error?
	while (!cgi_he_complete && ((handler.cgi_buf_pos)++ < handler.cgi_bytes_read))
	{
		ch = handler.cgi_buf[handler.cgi_buf_pos];

		switch (cgi_resp_he_state)
		{
			case he_start:
				if (ch == CR)
				{
					cgi_resp_he_state = he_end;
					break;
				}
				else if (ch == LF)
				{
					cgi_he_complete = 1;
					break;
				}
				else
					cgi_resp_he_state = he_name;

			case he_name: 
				if (ch == ':')
				{
					cgi_resp_he_state = he_ws;
					break;
				}
				else
				{
					header_name.append(1, ch);
					break;
				}

			case he_ws:
				if (ch == SP)
					break;
				else
					cgi_resp_he_state = he_value;
				
			case he_value:
				if (ch == CR)
					break;
				if (ch == LF)
					cgi_resp_he_state = he_done;
				else
				{
					header_value.append(1, ch);
					break;
				}

			case he_done:
				if (header_name == "Status")
				{
					std::string status;
					for (std::string::iterator it = header_value.begin(); it != header_value.end(); it++)
					{
						if (isdigit(*it))
							status += *it;
					}
					handler.setStatus(toInt(status));
					if (toInt(status) >= 400)
						throw CustomException("CGI Error");
				}
				cgi_header_fields.insert(std::pair<std::string, std::string>(header_name, header_value));
				header_name.clear();
				header_value.clear();
				cgi_resp_he_state = he_start;
				break;

			case he_end:
				if (ch == LF)
				{
					cgi_he_complete = 1;
					break;
				}
				handler.setStatus(500);
				throw CustomException("Internal Server Error");
		}
	}
}

void	CGIResponse::storeBody()
{
	handler.cgi_buf_pos++;
	int to_write = handler.cgi_bytes_read - handler.cgi_buf_pos;
	outfile.open(temp_body_filepath, std::ios::app | std::ios::binary);
	outfile.write(reinterpret_cast<const char*>(&handler.cgi_buf[handler.cgi_buf_pos]), to_write);
	handler.cgi_buf_pos += to_write;
	body_size += to_write;
	outfile.close();
}


bool	CGIResponse::processBuffer()
{
	if (!cgi_he_complete)
		readHeaderFields();
	if (cgi_he_complete)
	{

		if (cgi_header_fields.find("Location") != cgi_header_fields.end()) 
			return (1);
		else if (cgi_header_fields.find("Content-Type") != cgi_header_fields.end())
		{
			if (temp_body_filepath.empty())
				temp_body_filepath = createTmpFilePath();
			storeBody();
		}
		else if (cgi_header_fields.find("Status") == cgi_header_fields.end())
		{
			handler.setStatus(500);
			throw CustomException("Internal Server Error");
		}
	}
	return (0);
}

void	CGIResponse::createHeaderFields()
{
	for (std::map<std::string, std::string>::iterator it = cgi_header_fields.begin(); it != cgi_header_fields.end(); it++)
	{
		if (it->first != "Status" || it->first != "Content-Length") // also remove any non-standard HTTP-headers here (probably provide a list and check)
			header_fields.append(it->first + ": " + it->second + "\r\n");
	}
	header_fields.append("Content-Length: " + toString(body_size) + "\r\n");
	header_fields.append("\r\n");
}

void	CGIResponse::createResponse()
{
	status_line = createStatusLine();
	createHeaderFields();
	if (cgi_header_fields.find("Content-Type") != cgi_header_fields.end())
	{
		openBodyFile(temp_body_filepath);
		body = createBodyChunk();
		chunked_body = 1;
	}
}
