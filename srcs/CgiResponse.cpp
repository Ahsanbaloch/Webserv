
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
					cgi_resp_he_state = he_value;
					break;
				}
				else
				{
					header_name.append(1, ch);
					break;
				}
				
			case he_value:
				if (ch == SP)
					break;
				else if (ch == CR)
					break;
				if (ch == LF)
				{
					cgi_resp_he_state = he_done;
					break;
				}
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
				else
				{
					cgi_header_fields.insert(std::pair<std::string, std::string>(header_name, header_value));
					header_name.clear();
					header_value.clear();
				}
				cgi_resp_he_state = he_start;
				break;

			case he_end:
				if (ch == LF)
				{
					cgi_he_complete = 1;
					break;
				}
				handler.setStatus(500); // different error
				throw CustomException("Internal Server Error");
		}
	}
}

void	CGIResponse::storeBody()
{
	// also might do content length check
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
		for (std::map<std::string, std::string>::iterator it = cgi_header_fields.begin(); it != cgi_header_fields.end(); it++)
		{
			std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
		}

		if (cgi_header_fields.find("Location") != cgi_header_fields.end()) 
		{

			return (1);
			// check if the file specified by location is existing
				// if not do something
				// else perform internal redirect (probably based on the file type)
			

			// perform internal redirect
				// if internal redirect --> reset path, filename and file extension and research for location
				// find new Location
				// make getResponse
				// status code 303 (See other); needs to be sent in this case
			// close(cgi_handler->cgi_out[0]);
			// response->createResponse();
			// response_ready = 1;
		}
		else if (cgi_header_fields.find("Content-Type") != cgi_header_fields.end())
		{
			if (temp_body_filepath.empty())
				temp_body_filepath = "www/temp_body/testcgi.bin";
			storeBody();
		}
		else 
		{
			handler.setStatus(500);
			throw CustomException("CGI Error"); // should this be done?
		}
	}
	return (0);
}

void	CGIResponse::createHeaderFields()
{
	// for testing
	header_fields.append("Content-Type: text/html\r\n");
	header_fields.append("Content-Length: " + toString(body_size) + "\r\n");
	header_fields.append("\r\n");

}

void	CGIResponse::createResponse()
{
	status_line = createStatusLine();
	createHeaderFields();
	// if (header_fields.find("Location") != std::string::npos)
	// {
	// 	; // make internal redirect
	// }
	if (cgi_header_fields.find("Content-Type") != cgi_header_fields.end())
	{
		openBodyFile(temp_body_filepath);
		body = createBodyChunk();
		chunked_body = 1;
	}




	// openBodyFile(handler.getCGI()->getCGIOutput());
	// readCGIHeader();
	// if (header_fields.find("Location") != std::string::npos)
	// 	body = "";
	// else if (header_fields.find("Content-Type") != std::string::npos)
	// {
	// 	if (header_fields.find("Content-Length") == std::string::npos)
	// 		header_fields.append("Content-Length: " + toString(body_size) + "\r\n");
	// 	body = createBodyChunk();
	// 	chunked_body = 1;
	// }
	// else
	// {
	// 	handler.setStatus(500);
	// 	throw CustomException("CGI Error"); // should this be done?
	// }
	// header_fields.append("\r\n");
	// status_line = createStatusLine();

	// need to read the header line of the CGI response
	// needs to be at least one line seperated by newline
	// may include status code
		
		// Content-Type --> if html has been created (e.g. form); status: 200
		

	// if (handler.getHeaderInfo().getMethod() == "GET")
	// {
		//	Content-Type --> if html has been created (e.g. form); status: 200
	// }
	// else
	// {
	// 	handler.setStatus(200);

	// }
}
