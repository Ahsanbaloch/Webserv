
#include "RequestHeader.h"
#include "RequestHandler.h"

/////////////// CONSTRUCTORS & DESTRUCTORS //////////////////

RequestHeader::RequestHeader()
	: handler(*new RequestHandler())
{
	body_beginning = 0;
	body_length = 0;
	header_complete = 0;
	rl_parsing_done = 0;
	headers_parsing_done = 0;
	transfer_encoding_exists = 0;
	content_length_exists = 0;
	host_exists = 0;
	path_encoded = 0;
	query_encoded = 0;
	dot_in_path = 0;
	body_expected = 0;
	expect_exists = 0;
	headers_state = he_start;
	rl_state = rl_start;
}

RequestHeader::RequestHeader(RequestHandler& src)
	: handler(src)
{
	body_beginning = 0;
	body_length = 0;
	header_complete = 0;
	rl_parsing_done = 0;
	headers_parsing_done = 0;
	transfer_encoding_exists = 0;
	content_length_exists = 0;
	host_exists = 0;
	path_encoded = 0;
	query_encoded = 0;
	dot_in_path = 0;
	body_expected = 0;
	expect_exists = 0;
	headers_state = he_start;
	rl_state = rl_start;
}

RequestHeader::RequestHeader(const RequestHeader& src)
	: handler(src.handler)
{
	method = src.method;
	path = src.path;
	query = src.query;
	version = src.version;
	header_fields = src.header_fields;
	filename = src.filename;
	file_ext = src.file_ext;
	body_beginning = src.body_beginning;
	body_length = src.body_length;
	header_complete = src.header_complete;
	rl_parsing_done = src.rl_parsing_done;
	headers_parsing_done = src.headers_parsing_done;
	transfer_encoding_exists = src.transfer_encoding_exists;
	content_length_exists = src.content_length_exists;
	host_exists = src.host_exists;
	path_encoded = src.path_encoded;
	query_encoded = src.query_encoded;
	dot_in_path = src.dot_in_path;
	body_expected = src.body_expected;
	expect_exists = src.expect_exists;
	headers_state = src.headers_state;
	rl_state = src.rl_state;
}

RequestHeader& RequestHeader::operator=(const RequestHeader& src)
{
	if (this != &src)
	{
		method = src.method;
		path = src.path;
		query = src.query;
		version = src.version;
		header_fields = src.header_fields;
		filename = src.filename;
		file_ext = src.file_ext;
		body_beginning = src.body_beginning;
		body_length = src.body_length;
		header_complete = src.header_complete;
		rl_parsing_done = src.rl_parsing_done;
		headers_parsing_done = src.headers_parsing_done;
		transfer_encoding_exists = src.transfer_encoding_exists;
		content_length_exists = src.content_length_exists;
		host_exists = src.host_exists;
		path_encoded = src.path_encoded;
		query_encoded = src.query_encoded;
		dot_in_path = src.dot_in_path;
		body_expected = src.body_expected;
		expect_exists = src.expect_exists;
		headers_state = src.headers_state;
		rl_state = src.rl_state;
	}
	return (*this);
}

RequestHeader::~RequestHeader()
{
}

/////////////// GETTERS //////////////////

std::string	RequestHeader::getMethod() const
{
	return (method);
}

std::string RequestHeader::getHttpVersion() const
{
	return (version);
}

std::string RequestHeader::getQuery() const
{
	return (query);
}

std::string RequestHeader::getPath() const
{
	return (path);
}

bool	RequestHeader::getHeaderProcessingStatus() const
{
	return (header_complete);
}

bool	RequestHeader::getRequestLineStatus() const
{
	return (rl_parsing_done);
}

std::map<std::string, std::string>	RequestHeader::getHeaderFields() const
{
	return (header_fields);
}

std::string	RequestHeader::getFilename() const
{
	return (filename);
}

std::string RequestHeader::getFileExtension() const
{
	return (file_ext);
}

bool	RequestHeader::getBodyExpectanceStatus() const
{
	if (method == "POST")
		return (body_expected);
	else
		return (0);
}

bool	RequestHeader::getHeaderExpectedStatus() const
{
	return (expect_exists);
}

bool	RequestHeader::getTEStatus() const
{
	return (transfer_encoding_exists);
}

int		RequestHeader::getBodyBeginning() const
{
	return (body_beginning);
}

int		RequestHeader::getBodyLength() const
{
	return (body_length);
}


/////////////// SETTERS //////////////////

void	RequestHeader::makeInternalRedirect(std::string new_path)
{
	path = new_path;
	identifyFileName();
}


/////////////// MAIN METHODS //////////////////

void	RequestHeader::identifyFileName()
{
	std::size_t found = path.find_first_of('.');
	if (found == std::string::npos)
	{
		filename = "";
		file_ext = "";
	}
	else
	{
		std::size_t extra_path_found = path.substr(found).find_first_of('/');
		if (extra_path_found == std::string::npos)
		{
			file_ext = path.substr(found);
			found = path.find_last_of('/');
			filename = path.substr(found + 1);
		}
		else
		{
			file_ext = path.substr(found, extra_path_found);
			found = path.substr(0, found).find_last_of('/');
			extra_path_found = path.substr(found + 1).find_last_of('/');
			filename = path.substr(found + 1, extra_path_found);
		}
	}
}

void	RequestHeader::checkHeader()
{
	checkFields();
	if (dot_in_path)
		removeDots();
	if (path_encoded)
		decode(path);
	if (query_encoded)
		decode(query);
}

void	RequestHeader::checkFields()
{
	if (header_fields.find("content-length") != header_fields.end() && method == "POST")
	{
		if (body_length == 0 && header_fields.find("transfer-encoding") == header_fields.end())
		{
			handler.setStatus(400);
			throw CustomException("Bad request: POST without body");
		}
	}
	if (header_fields.find("host") == header_fields.end())
	{
		handler.setStatus(400);
		throw CustomException("Bad request: no host field found");
	}
	if (!transfer_encoding_exists && !content_length_exists && method == "POST")
	{
		handler.setStatus(411);
		throw CustomException("Length Required");
	}
	if (content_length_exists && body_length > handler.getSelectedServer().bodySize)
	{
		handler.setStatus(413);
		throw CustomException("Content Too Large");
	}
}

void	RequestHeader::decode(std::string& sequence)
{
	for (std::string::iterator it = sequence.begin(); it != sequence.end(); it++)
	{
		if (*it == '%')
		{
			sequence.erase(it);
			unsigned int decoded_char;
			std::stringstream ss;
			if ((*it >= '0' && *it <= '9') || (*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z'))
			{
				ss << std::hex << *it;
				sequence.erase(it);
			}
			else
			{
				handler.setStatus(400);
				throw CustomException("Bad request: encoding error");
			}
			if ((*it >= '0' && *it <= '9') || (*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z'))
			{
				ss << std::hex << *it;
				ss >> decoded_char;
				*it = decoded_char;
			}
			else
			{
				handler.setStatus(400);
				throw CustomException("Bad request: encoding error");
			}
		}
		if (*it == '+')
			*it = ' ';
	}
}

void	RequestHeader::removeDots()
{
	std::vector<std::string> updated_path;
	std::vector<std::string> parts = splitPath(path, '/');

	if (parts.size() > 1)
		parts.erase(parts.begin());

	for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); it++)
	{
		if (*it == "/.")
			continue;
		else if (*it == "/.." && updated_path.size() > 0)
			updated_path.pop_back();
		else if (*it != "/..")
		{
			updated_path.push_back(*it);
		}
	}
	path.clear();
	for (std::vector<std::string>::iterator it = updated_path.begin(); it != updated_path.end(); it++)
		path.append(*it);
	if (path.empty())
		path.append("/");
}


void	RequestHeader::checkBodyLength(std::string value)
{
	for(std::string::iterator it = value.begin(); it != value.end(); it++)
	{
		if (*it == '.')
		{
			value.erase(*it);
			it++;
			for (;it != value.end(); it++)
			{
				if (isdigit(*it))
					value.erase(*it);
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when parsing content-length");
				}
			}
		}
		else if (!isdigit(*it))
		{
			handler.setStatus(400);
			throw CustomException("Bad request: detected when parsing content-length");
		}
	}
	body_length = toInt(value);
}

void	RequestHeader::parseHeaderFields()
{
	unsigned char	ch;
	std::string		header_name = "";
	std::string		header_value = "";

	while (!headers_parsing_done && (handler.buf_pos)++ < handler.getBytesRead())
	{
		ch = handler.buf[handler.buf_pos];
	
		switch (headers_state) 
		{
			case he_start:
				if (ch == CR)
				{
					headers_state = hes_end;
					break;
				}
				else if (ch == LF)
				{
					headers_parsing_done = 1;
					body_beginning = handler.buf_pos;
					header_complete = 1;
					break;
				}
				else
					headers_state = he_name;
					// fall through
			case he_name:
				if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_')
				{
					if (ch >= 'A' && ch <= 'Z')
						header_name.append(1, std::tolower(ch));
					else
						header_name.append(1, ch);
				}
				else if (ch == ':')
					headers_state = he_ws_before_value;
				else if (ch == CR)
					headers_state = he_cr;
				else if (ch == LF)
					headers_state = he_done;
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when parsing header field name");
				}
				break;

			case he_ws_before_value:
				if (ch == SP)
					break;
				else if (ch == CR)
				{
					headers_state = he_cr;
					break;
				}
				else if (ch == LF)
				{
					headers_state = he_done;
					break;
				}
				else
					headers_state = he_value;
					// fall through
			case he_value:
				if (ch == SP)
					headers_state = he_ws_after_value;
				else if (ch == CR)
					headers_state = he_cr;
				else if (ch == LF)
					headers_state = he_done;
				else if (ch > 32 && ch < 127)
					header_value.append(1, ch);
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when parsing header field value");
				}
				break;
			
			case he_ws_after_value:
				if (ch == SP)
					break;
				else if (ch == CR)
				{
					headers_state = he_cr;
					break;
				}
				else if (ch == LF)
				{
					headers_state = he_done;
					break;
				}
				else
				{
					headers_state = he_value;
					header_value.append(1, SP);
					header_value.append(1, ch);
					break;
				}

			case he_cr:
				if (ch == LF)
					headers_state = he_done;
				else 
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when parsing header fields");
				}
				// fall through
			case he_done:
				if (header_name == "content-length")
				{
					if (content_length_exists) 
					{
						handler.setStatus(400);
						throw CustomException("Bad request: dectected multiple content length headers");
					}
					content_length_exists = 1;
					checkBodyLength(header_value);
					if (body_length > 0)
						body_expected = 1;
				}
				if (header_name == "transfer-encoding")
				{
					if (transfer_encoding_exists)
					{
						handler.setStatus(400);
						throw CustomException("Bad request: dectected multiple transfer-encoding headers");
					}
					if (header_value != "chunked")
					{
						handler.setStatus(501);
						throw CustomException("Not implemented: only 'chunked' allowed as transfer-encoding");
					}
					transfer_encoding_exists = 1;
					body_expected = 1;
				}
				if (header_name == "host")
				{
					if (host_exists)
					{
						handler.setStatus(400);
						throw CustomException("Bad request: more than one entry for host detected");
					}
					if (header_value.empty())
					{
						handler.setStatus(400);
						throw CustomException("Bad request: value for host is empty");
					}
					host_exists = 1;
				}
				if (header_name == "expect")
					expect_exists = 1;
				header_fields.insert(std::pair<std::string, std::string>(header_name, header_value));
				header_name.clear();
				header_value.clear();
				headers_state = he_start;
				break;

			case hes_end:
				if (ch == LF)
				{
					headers_parsing_done = 1;
					body_beginning = handler.buf_pos;
					header_complete = 1;
					break;
				}
				handler.setStatus(400);
				throw CustomException("Bad request: detected when parsing header fields");
		}
	}

	if (!headers_parsing_done)
	{
		handler.setStatus(431);
		throw CustomException("Request Header Fields Too Large");
	}
}

void	RequestHeader::checkMethod()
{
	switch (handler.buf[handler.buf_pos])
	{
		case 'G':
			if (handler.buf[++(handler.buf_pos)] == 'E' && handler.buf[++(handler.buf_pos)] == 'T')
				method = "GET";
			else
			{
				if (handler.buf[handler.buf_pos] == '\0')
					break;
				handler.setStatus(501);
				throw CustomException("Not implemented: method requested is not implemented");
			}
			break;
		
		case 'P':
			if (handler.buf[++(handler.buf_pos)] == 'O' && handler.buf[++(handler.buf_pos)] == 'S' && handler.buf[++(handler.buf_pos)] == 'T')
				method = "POST";
			else
			{
				if (handler.buf[handler.buf_pos] == '\0')
					break;
				handler.setStatus(501);
				throw CustomException("Not implemented: method requested is not implemented");
			}
			break;

		case 'D':
			if (handler.buf[++(handler.buf_pos)] == 'E' && handler.buf[++(handler.buf_pos)] == 'L' && handler.buf[++(handler.buf_pos)] == 'E' && handler.buf[++(handler.buf_pos)] == 'T' && handler.buf[++(handler.buf_pos)] == 'E')
				method = "DELETE";
			else
			{
				if (handler.buf[handler.buf_pos] == '\0')
					break;
				handler.setStatus(501);
				throw CustomException("Not implemented: method requested is not implemented");
			}
			break;
		default:
			handler.setStatus(501);
			throw CustomException("Not implemented: method requested is not implemented");
			break;
		}
}

void	RequestHeader::checkHttpVersion()
{
	if (handler.buf[handler.buf_pos] == 'H' && handler.buf[++(handler.buf_pos)] == 'T' && handler.buf[++(handler.buf_pos)] == 'T' && handler.buf[++(handler.buf_pos)] == 'P' && handler.buf[++(handler.buf_pos)] == '/' && handler.buf[++(handler.buf_pos)] == '1'
		&& handler.buf[++(handler.buf_pos)] == '.' && handler.buf[++(handler.buf_pos)] == '1')
	{
		version = "HTTP/1.1";
		handler.buf_pos++;
	}
	else
	{
		if (handler.buf[handler.buf_pos] == '\0')
			return ;
		handler.setStatus(505);
		throw CustomException("HTTP Version Not Supported");
	}
}

void	RequestHeader::handleMultipleSlashes()
{
	while(handler.buf[handler.buf_pos] == '/')
		handler.buf_pos++;
	handler.buf_pos--;
}

void	RequestHeader::parseRequestLine()
{
	unsigned char	ch;

	while (!rl_parsing_done && (handler.buf_pos)++ < handler.getBytesRead())
	{
		ch = handler.buf[handler.buf_pos];
	
		switch (rl_state) 
		{
			case rl_start:
				if (handler.buf_pos == 0 && ch == CR)
					break;
				if (ch == LF)
					break;
				rl_state = rl_method;
				// fall through
			case rl_method:
				checkMethod();
				rl_state = rl_first_divider;
				break;

			case rl_first_divider:
				if (ch == SP)
					break;
				else if (ch == '/')
				{
					rl_state = rl_path;
					handler.buf_pos--;
					break;
				}
				handler.setStatus(400);
				throw CustomException("Bad request: detected when parsing request line");
				
			case rl_path:
				switch (ch)
				{
					case SP:
						rl_state = rl_http;
						break;
					case CR:
						rl_state = rl_cr;
						break;
					case LF:
						rl_state = rl_done;
						break;
					case '.':
						if (handler.buf[handler.buf_pos - 1] == '/')
							dot_in_path = 1;
						break;
					case '%':
						path_encoded = 1;
						break;
					case '/':
						handleMultipleSlashes();
						break;
					case '?':
						rl_state = rl_query;
						break;
					case '+':
						path_encoded = 1;
						break;
					default:
						if (ch < 32 || ch == 127)
						{
							handler.setStatus(400);
							throw CustomException("Bad request: detected when parsing request line");
						}
						break;
				}
				if (rl_state == rl_path)
					path.append(1, ch);
				break;

			case rl_query:
				switch (ch)
				{
					case SP:
						rl_state = rl_http;
						break;
					case CR:
						rl_state = rl_cr;
						break;
					case LF:
						rl_state = rl_done;
						break;
					case '%':
						query_encoded = 1;
						break;
					case '+':
						query_encoded = 1;
						break;
					default:
						if (ch < 32 || ch == 127)
						{
							handler.setStatus(400);
							throw CustomException("Bad request: detected when parsing request line query");
						}
						break;
				}
				if (rl_state == rl_query)
					query.append(1, ch);
				break;

			case rl_http: 
				if (ch == CR)
				{
					rl_state = rl_cr;
					break;
				}
				if (ch == LF)
				{
					rl_state = rl_done;
					break;
				}
				if (ch == 'H')
					checkHttpVersion();
				break;

			case rl_cr:
				if (ch == LF)
				{
					rl_state = rl_done;
					break;
				}
				else 
				{
					handler.setStatus(400);
					throw CustomException("Bad request: detected when parsing request line");
				}
			
			case rl_done:
				rl_parsing_done = 1;
				handler.buf_pos--;
				break;
		}
	}

	if (!rl_parsing_done)
	{
		handler.setStatus(414);
		throw CustomException("Request-URI Too Long");
	}
}
