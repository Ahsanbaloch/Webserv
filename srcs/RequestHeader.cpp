
#include "RequestHeader.h"
#include "RequestHandler.h"

/////////////// CONSTRUCTORS & DESTRUCTORS //////////////////

RequestHeader::RequestHeader()
	: handler(*new RequestHandler()) // is there a better way to initialize the RequestHeader reference?
{
	// is there a better way to initialize the variables?
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
	// is there a better way to initialize the variables?
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
	// is there a better way to initialize the variables?
	method = src.method;
	path = src.path;
	query = src.query;
	version = src.version;
	header_fields = src.header_fields;
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

bool	RequestHeader::getHeaderStatus() const
{
	return (header_complete);
}

std::map<std::string, std::string>	RequestHeader::getHeaderFields() const
{
	return (header_fields);
}

bool	RequestHeader::getBodyStatus() const
{
	return (body_expected);
}

bool	RequestHeader::getHeaderExpectedStatus() const
{
	return (expect_exists);
}

bool	RequestHeader::getTEStatus() const
{
	return (transfer_encoding_exists);
}

/////////////// MAIN METHODS //////////////////

void	RequestHeader::parseHeader()
{
	if (!rl_parsing_done)
		parseRequestLine();
	if (rl_parsing_done)
		parseHeaderFields();
}

void	RequestHeader::checkHeader()
{
	if (dot_in_path)
		removeDots();
	// Decoding to do?: A common defense against response splitting is to filter requests for data that looks like encoded CR and LF (e.g., "%0D" and "%0A") --> What to do then?
	if (path_encoded)
		decode(path);
	if (query_encoded)
		decode(query);
	checkFields();
	std::cout << "RequestHeader parsing complete\n";
}

void	RequestHeader::checkFields()
{
	// others check such as empty host field value, TE != chunked etc. is done in parsing
	if (header_fields.find("host") == header_fields.end()) // is is even connecting without host field?
	{
		handler.setStatus(410);
		throw CustomException("Bad request 1");
	}
	if (!transfer_encoding_exists && !content_length_exists && method == "POST") // if both exist at the same time is check when parsing
	{
		handler.setStatus(411);
		throw CustomException("Length Required");
	}
}

void	RequestHeader::decode(std::string& sequence)
{
	for (std::string::iterator it = sequence.begin(); it != sequence.end(); it++)  // allowed values #01 - #FF (although ASCII only goes till #7F/7E)
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
				throw CustomException("decoding error");
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
				throw CustomException("decoding error");
			}
		}
		if (*it == '+')
			*it = ' ';
	}
}

void	RequestHeader::removeDots()
{
	std::vector<std::string> updated_path;
	std::vector<std::string> parts = handler.splitPath(path, '/');

	if (parts.size() > 1)
		parts.erase(parts.begin());

	for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); it++)
	{
		std::cout << "part: " << *it << std::endl;
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
	// elaborate check --> see RFC for what is an accepted format for providing the length of the body

	std::cout << "Value: " << value << std::endl;
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
					throw CustomException("Bad request 2"); // other error code?
				}
			}
		}
		else if (!isdigit(*it))
		{
			handler.setStatus(400);
			throw CustomException("Bad request 3"); // other error code?
		}
	}
	handler.body_length = std::atoi(value.c_str());
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
					break;
				}
				else
					headers_state = he_name;
			
			case he_name: // currently not allowing whitespace preceding the field name // allowed chars: //  A-Z, a-z, 0–9, hyphen ( - ), or underscore ( _ ).
				if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_')
				{
					if (ch >= 'A' && ch <= 'Z')
						header_name.append(1, std::tolower(ch)); // because not case sensitive, we make all characters lowercase to make string comparsions easier later on
					else
						header_name.append(1, static_cast<char>(ch));
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
					throw CustomException("Bad request 4");
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


			case he_value:
				if (ch == SP)
					headers_state = he_ws_after_value;
				else if (ch == CR)
					headers_state = he_cr;
				else if (ch == LF)
					headers_state = he_done;
				// else if (ch == '%')
				// {
				// 	field_encoded = 1;
				// 	fields_encoded.push_back(header_name);
				// 	header_value.append(1, ch);
				// }
				else if (ch > 32 && ch < 127) // accepted: a-z, A-Z, and 0-9 _ :;.,\/"'?!(){}[]@<>=-+*#$&`|~^%
					header_value.append(1, ch);
				else
				{
					handler.setStatus(400);
					throw CustomException("Bad request 5");
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
					header_value.append(1, static_cast<char>(ch));
					break;
				}

			case he_cr:
				if (ch == LF)
					rl_state = rl_done;
				else 
				{
					handler.setStatus(400);
					throw CustomException("Bad request 6");
				}
			
			case he_done:
				// std::cout << "header line fully parsed\n";
				// check if there is a body in the message
				if (header_name == "content-length")
				{
					// if (content_length_exists || transfer_encoding_exists) // for security: when content_length is specified, TE shouldn't be
					// {
					// 	// to reduce attack vectors for request smuggling, we don't allow multiple content_length headers
					// 	handler.setStatus(400); // correct error value
					// 	throw CustomException("Bad request");
					// }
					content_length_exists = 1;
					checkBodyLength(header_value);
					if (handler.body_length > 0)
						body_expected = 1;
				}
				if (header_name == "transfer-encoding")
				{
					// if (transfer_encoding_exists || content_length_exists) // // for security: when content_length is specified, TE shouldn't be
					// {
					// 	// to reduce attack vectors for request smuggling, we don't allow multiple TE headers
					// 	handler.setStatus(400); // correct error value
					// 	throw CustomException("Bad request");
					// }
					if (header_value != "chunked")
					{
						handler.setStatus(501);
						throw CustomException("Not implemented 1");
					}
					transfer_encoding_exists = 1;
					body_expected = 1;
				}
				if (header_name == "host")
				{
					if (host_exists)
					{
						handler.setStatus(400);
						throw CustomException("Bad request 7");
					}
					if (header_value.empty())
					{
						handler.setStatus(400);
						throw CustomException("Bad request 8");
					}
					host_exists = 1;
				}
				if (header_name == "expect")
					expect_exists = 1; // in this case a response is expected before the (rest of) body is sent
				header_fields.insert(std::pair<std::string, std::string>(header_name, header_value));
				header_name.clear();
				header_value.clear();
				headers_state = he_start;
				break;

			case hes_end:
				if (ch == LF)
				{
					headers_parsing_done = 1;
					header_complete = 1;
					// std::cout << "headers fully parsed\n";
					break;
				}
				handler.setStatus(400);
				throw CustomException("Bad request 9");
		}
	}

	if (!headers_parsing_done) // is this the correct location to check?
	{
		handler.setStatus(413); // correct error code when header is too large for buffer OR 431 Request Header Fields Too Large
		throw CustomException("Content Too Large");  // correct error code when header is too large for buffer
	}

	// A sender MUST NOT send whitespace between the start-line and the first header field.
	// A recipient that receives whitespace between the start-line and the first header field MUST either reject the message as invalid 
	// or consume each whitespace-preceded line without further processing of it (i.e., ignore the entire line, along with any subsequent lines preceded by whitespace, until a properly formed header field is received or the header section is terminated).
	
	// Each field line consists of a case-insensitive field name followed by a colon (":"), optional leading whitespace, the field line value, and optional trailing whitespace.
	// No whitespace is allowed between the field name and colon --> 400 bad request

	// A field line value might be preceded and/or followed by optional whitespace (OWS);
	// The field line value does not include that leading or trailing whitespace
	// line folding in the the "message/http" media type???

	// end is signified by CRLFCRLF
}

// void	Header::checkMethod(unsigned char* buf, int* buf_pos)
void	RequestHeader::checkMethod()
{
	switch (handler.buf[handler.buf_pos])
	{
		case 'G':
			// check if next characters are E and T
			if (handler.buf[++(handler.buf_pos)] == 'E' && handler.buf[++(handler.buf_pos)] == 'T')
				method = "GET";
			else
			{
				handler.setStatus(501);
				throw CustomException("Not implemented 2");
			}
			break;
		
		case 'P':
			// check if next characters are O S and T
			if (handler.buf[++(handler.buf_pos)] == 'O' && handler.buf[++(handler.buf_pos)] == 'S' && handler.buf[++(handler.buf_pos)] == 'T')
				method = "POST";
			else
			{
				handler.setStatus(501);
				throw CustomException("Not implemented 3");
			}
			break;

		case 'D':
			// check of next characters are E L E T E
			if (handler.buf[++(handler.buf_pos)] == 'E' && handler.buf[++(handler.buf_pos)] == 'L' && handler.buf[++(handler.buf_pos)] == 'E' && handler.buf[++(handler.buf_pos)] == 'T' && handler.buf[++(handler.buf_pos)] == 'E')
				method = "DELETE";
			else
			{
				handler.setStatus(501);
				throw CustomException("Not implemented 4");
			}
			break;
		default:
			handler.setStatus(501);
			throw CustomException("Not implemented 5");
			break;
		}
}

// void	Header::checkHttpVersion(unsigned char* buf, int* buf_pos)
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
		handler.setStatus(505);
		throw CustomException("HTTP Version Not Supported");
	}
}

void	RequestHeader::handleMultipleSlashes() // could probably also be a created as a special state
{
	while(handler.buf[handler.buf_pos] == '/')
		handler.buf_pos++;
	handler.buf_pos--;
}

void	RequestHeader::parseRequestLine()
{
	unsigned char	ch;

	// implement max request line rule? (414 and 501 errors)
	// do we need to check for scheme and authority?

	// reserved chars in URI: ! * ' ( ) ; : @ & = + $ , / ? % # [ ]
	// special meanings: + SP / ? % #   & (query)  = (query)  @ (authority)  : (authority)
	// unreseved:  - _ . ~
	// alphanumeric

	while (!rl_parsing_done && (handler.buf_pos)++ < handler.getBytesRead())
	{
		ch = handler.buf[handler.buf_pos];
	
		switch (rl_state) 
		{
			// in the interest of robustness, ignore at least one empty line
			// double check if a single LF occurance is accepted // is there a space afterwards?
			case rl_start:
				if (handler.buf_pos == 0 && ch == CR)
					break;
				if (ch == LF)
					break;
				rl_state = rl_method;

			case rl_method: // can this also be termined by CRLF?
				// checkMethod(buf, buf_pos);
				checkMethod();
				rl_state = rl_first_divider;
				break;

			case rl_first_divider: // may add to method check
				if (ch == SP) // do we want to allow multiple spaces?
					break;
				else if (ch == '/')
				{
					rl_state = rl_path;
					handler.buf_pos--;
					break;
				}
				handler.setStatus(400);
				throw CustomException("Bad request 10");
				
			case rl_path:
				// do we have to handle authority request lines?
				// query line goes to cgi --> Query string env variable

				switch (ch)
				{
					case SP:
						rl_state = rl_http;
						break;
					case CR: // what if this occurs and no http version is specified?
						rl_state = rl_cr;
						break;
					case LF: // what if this occurs and no http version is specified?
						rl_state = rl_done;
						break;
					case '.': // nginx does not allow two dots at the beginning if nothing comes after; a single dot leads to index (also on some websites with two dots); three dots to file not found
						if (handler.buf[handler.buf_pos - 1] == '/')
							dot_in_path = 1;
						break;
					case '%':
						path_encoded = 1; // when interpreting request needs to be decoded
						// rl_state = rl_percent_encoded; // need to be followed by numercial code that needs to be interpreted --> otherwise 400 bad request (not the case for query)
						break;
					case '/': // checking with nginx, there can be several slashes after each other
						handleMultipleSlashes();
						break;
					case '?':
						rl_state = rl_query;
						break;
					case '+': // means space; needs to be decoded when interpreting request
						path_encoded = 1;
						break;
					default:
						if (ch < 32 || ch == 127)
						{
							handler.setStatus(400);
							throw CustomException("Bad request 11");
						}
						break;

					// case '#': // in what case is this character even encountered in a http message? probably only in the query part and when interpreting // fragments are not sent in the http request messages. The reason is that the fragment identifier is only used by the browser

				}
				if (rl_state == rl_path)
					path.append(1, static_cast<char>(ch));
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
							throw CustomException("Bad request 12");
						}
						break;
				}
				if (rl_state == rl_query)
					query.append(1, static_cast<char>(ch));
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
					throw CustomException("Bad request 13");
				}
			
			case rl_done:
				// std::cout << "request line fully parsed\n";
				rl_parsing_done = 1;
				handler.buf_pos--; // Why does this needs to be done?
				break;
		}
		// check for URI (path) --> request target (origin-form)
			// doesn't allow any whitespace
			// if invalid request-line
				// respond with 400 (Bad Request) and close connection

		// enforce single-space grammar rule? --> check how nginx implements it
			// check HTTP version
	}

	if (!rl_parsing_done)
	{
		handler.setStatus(414);
		throw CustomException("Request-URI Too Long");
	}

}
