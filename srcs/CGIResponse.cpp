
#include "CGIResponse.h"


///////// CONSTRUCTORS & DESTRUCTORS ///////////

CGIResponse::CGIResponse(RequestHandler& src)
	: AResponse(src)
{
	cgi_he_complete = 0;
	cgi_resp_he_state = he_start;
	valid_headers = getValidHTTPHeaders();
}

CGIResponse::CGIResponse()
	: AResponse()
{
	cgi_he_complete = 0;
	cgi_resp_he_state = he_start;
	valid_headers = getValidHTTPHeaders();
}

CGIResponse::~CGIResponse()
{
}

CGIResponse::CGIResponse(const CGIResponse& src)
	:	AResponse(src)
{
	cgi_header_fields = src.cgi_header_fields;
	temp_body_filepath = src.temp_body_filepath;
	cgi_he_complete = src.cgi_he_complete;
	cgi_resp_he_state = src.cgi_resp_he_state;
	valid_headers = src.valid_headers;
}

CGIResponse&	CGIResponse::operator=(const CGIResponse& src)
{
	if (this != &src)
	{
		AResponse::operator=(src);
		cgi_header_fields = src.cgi_header_fields;
		temp_body_filepath = src.temp_body_filepath;
		cgi_he_complete = src.cgi_he_complete;
		cgi_resp_he_state = src.cgi_resp_he_state;
		valid_headers = src.valid_headers;
	}
	return (*this);
}


///////// GETTERS ///////////

std::string	CGIResponse::getTempBodyFilePath() const
{
	return (temp_body_filepath);
}

std::map<std::string, std::string>	CGIResponse::getCGIHeaderFields() const
{
	return (cgi_header_fields);
}


///////// HELPER METHODS ///////////

void	CGIResponse::readHeaderFields()
{
	unsigned char	ch;
	std::string		header_name = "";
	std::string		header_value = "";

	while (!cgi_he_complete && ((handler.cgi_buf_pos)++ < handler.getCGIBytesRead()))
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
					// fall through
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
					// fall through
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
				// fall through
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
						throw CustomException("Error indicated by CGI: Sending error response");
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
				throw CustomException("Internal Server Error: detected when parsing CGI header");
		}
	}
}

void	CGIResponse::storeBody()
{
	handler.cgi_buf_pos++;
	int to_write = handler.getCGIBytesRead() - handler.cgi_buf_pos;
	outfile.open(temp_body_filepath.c_str(), std::ios::app | std::ios::binary);
	outfile.write(reinterpret_cast<const char*>(&handler.cgi_buf[handler.cgi_buf_pos]), to_write);
	handler.cgi_buf_pos += to_write;
	body_size += to_write;
	outfile.close();
}

std::vector<std::string>	CGIResponse::getValidHTTPHeaders()
{
	std::vector<std::string> valid_headers_temp;

	valid_headers_temp.push_back("Accept-CH");
	valid_headers_temp.push_back("Access-Control-Allow-Origin");
	valid_headers_temp.push_back("Access-Control-Allow-Credentials");
	valid_headers_temp.push_back("Access-Control-Expose-Headers");
	valid_headers_temp.push_back("Access-Control-Max-Age");
	valid_headers_temp.push_back("Access-Control-Allow-Methods");
	valid_headers_temp.push_back("Access-Control-Allow-Headers");
	valid_headers_temp.push_back("Accept-Patch");
	valid_headers_temp.push_back("Accept-Ranges");
	valid_headers_temp.push_back("Age");
	valid_headers_temp.push_back("Allow");
	valid_headers_temp.push_back("Alt-Svc");
	valid_headers_temp.push_back("Cache-Control");
	valid_headers_temp.push_back("Connection");
	valid_headers_temp.push_back("Content-Disposition");
	valid_headers_temp.push_back("Content-Encoding");
	valid_headers_temp.push_back("Content-Language");
	valid_headers_temp.push_back("Content-Location");
	valid_headers_temp.push_back("Content-MD5");
	valid_headers_temp.push_back("Content-Range");
	valid_headers_temp.push_back("Content-Type");
	valid_headers_temp.push_back("Date");
	valid_headers_temp.push_back("Delta-Base");
	valid_headers_temp.push_back("ETag");
	valid_headers_temp.push_back("Expires");
	valid_headers_temp.push_back("IM");
	valid_headers_temp.push_back("Last-Modified");
	valid_headers_temp.push_back("Link");
	valid_headers_temp.push_back("Location");
	valid_headers_temp.push_back("P3P");
	valid_headers_temp.push_back("Pragma");
	valid_headers_temp.push_back("Preference-Applied");
	valid_headers_temp.push_back("Proxy-Authenticate");
	valid_headers_temp.push_back("Public-Key-Pins");
	valid_headers_temp.push_back("Retry-After");
	valid_headers_temp.push_back("Server");
	valid_headers_temp.push_back("Set-Cookie");
	valid_headers_temp.push_back("Strict-Transport-Security");
	valid_headers_temp.push_back("Trailer");
	valid_headers_temp.push_back("Transfer-Encoding");
	valid_headers_temp.push_back("Tk");
	valid_headers_temp.push_back("Upgrade");
	valid_headers_temp.push_back("Vary");
	valid_headers_temp.push_back("Via");
	valid_headers_temp.push_back("Warning");
	valid_headers_temp.push_back("WWW-Authenticate");
	valid_headers_temp.push_back("X-Frame-Options");

	return (valid_headers_temp);
}

void	CGIResponse::createHeaderFields()
{
	for (std::map<std::string, std::string>::iterator it = cgi_header_fields.begin(); it != cgi_header_fields.end(); it++)
	{
		if (find(valid_headers.begin(), valid_headers.end(), it->first) != valid_headers.end())
			header_fields.append(it->first + ": " + it->second + "\r\n");
	}
	header_fields.append("Content-Length: " + toString(body_size) + "\r\n");
	header_fields.append("\r\n");
}


///////// MAIN METHODS ///////////

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
			throw CustomException("Internal Server Error: CGI doesn't send required header");
		}
	}
	return (0);
}
