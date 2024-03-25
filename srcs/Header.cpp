
#include "Header.h"
#include "RequestHandler.h"

Header::Header(RequestHandler& handler)
	: handler2(handler)
{
	rl_parsing_done = 0;
	headers_parsing_done = 0;
	transfer_encoding_exists = 0;
	content_length_exists = 0;
	host_exists = 0;
	expect_exists = 0;
	path_encoded = 0;
	query_encoded = 0;
	dot_in_path = 0;
	// field_encoded = 0;
	error = 0;
	body_length = 0;

	header_complete = 0;
	body_beginning = 0;

	method = ""; // does this reset the string?
	redirected_path = "";
	path = ""; // does this reset the string?
	query = ""; // does this reset the string?
	version = ""; // does this reset the string?
}


// Header::Header(/* args */)
// {
// 	rl_parsing_done = 0;
// 	headers_parsing_done = 0;
// 	transfer_encoding_exists = 0;
// 	content_length_exists = 0;
// 	host_exists = 0;
// 	expect_exists = 0;
// 	path_encoded = 0;
// 	query_encoded = 0;
// 	dot_in_path = 0;
// 	// field_encoded = 0;
// 	error = 0;
// 	body_length = 0;

// 	header_complete = 0;
// 	body_beginning = 0;

// 	method = ""; // does this reset the string?
// 	redirected_path = "";
// 	path = ""; // does this reset the string?
// 	query = ""; // does this reset the string?
// 	version = ""; // does this reset the string?
// }

Header::~Header()
{
}

void	Header::checkFields()
{
	// others check such as empty host field value, TE != chunked etc. is done in parsing
	if (header_fields.find("host") == header_fields.end()) // is is even connecting without host field?
	{
		error = 410;
		throw CustomException("Bad request");
	}
	if (!transfer_encoding_exists && !content_length_exists && method == "POST") // if both exist at the same time is check when parsing
	{
		error = 411;
		throw CustomException("Length Required");
	}
}

void	Header::decodeRequestLine(std::string& sequence)
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
				error = 400;
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
				error = 400;
				throw CustomException("decoding error");
			}
		}
		if (*it == '+')
			*it = ' ';
	}
}


void	Header::decode() // A common defense against response splitting is to filter requests for data that looks like encoded CR and LF (e.g., "%0D" and "%0A") --> What to do then?
{
	// std::cout << "path before decoding: " << path << std::endl;
	// std::cout << "query: " << query << std::endl;
	if (path_encoded)
		decodeRequestLine(path);
	if (query_encoded)
		decodeRequestLine(query);
	// if (field_encoded)
	// {
	// 	for (std::vector<std::string>::iterator it = fields_encoded.begin(); it != fields_encoded.end(); it++)
	// 	{
	// 		decodeRequestLine(header_fields[*it]);
	// 	}
	// }

	// std::cout << "path after decoding: " << path << std::endl;
	// std::cout << "query after decoding: " << query << std::endl;
}


std::vector<std::string>	Header::splitPath(std::string input, char delim)
{
	std::istringstream			iss(input);
	std::string					item;
	std::vector<std::string>	result;
	
	while (std::getline(iss, item, delim))
		result.push_back("/" + item);
	return (result);
}

void	Header::removeDots()
{
	std::vector<std::string> updated_path;
	std::vector<std::string> parts = splitPath(path, '/');

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


void	Header::checkBodyLength(std::string value)
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
					error = 400;
					throw CustomException("Bad request"); // other error code?
				}
			}
		}
		else if (!isdigit(*it))
		{
			error = 400;
			throw CustomException("Bad request"); // other error code?
		}
	}
	body_length = std::atoi(value.c_str());
}

void	Header::parseHeaderFields(RequestHandler& handler)
{
	(void)handler;
	unsigned char	ch;
	std::string		header_name = "";
	std::string		header_value = "";

	headers_state = he_start; // move to constructor?

	while (!headers_parsing_done && (handler2.buf_pos)++ < handler2.bytes_read)
	{
		ch = handler2.buf[handler2.buf_pos];
	
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
					error = 400;
					throw CustomException("Bad request");
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
					error = 400;
					throw CustomException("Bad request");
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
					error = 400;
					throw CustomException("Bad request");
				}
			
			case he_done:
				std::cout << "header line fully parsed\n";
				// check if there is a body in the message
				if (header_name == "content-length")
				{
					if (content_length_exists || transfer_encoding_exists) // for security: when content_length is specified, TE shouldn't be
					{
						// to reduce attack vectors for request smuggling, we don't allow multiple content_length headers
						error = 400; // correct error value
						throw CustomException("Bad request");
					}
					content_length_exists = 1;
					checkBodyLength(header_value);
					if (body_length > 0)
						handler2.body_expected = 1;
				}
				if (header_name == "transfer-encoding")
				{
					if (transfer_encoding_exists || content_length_exists) // // for security: when content_length is specified, TE shouldn't be
					{
						// to reduce attack vectors for request smuggling, we don't allow multiple TE headers
						error = 400; // correct error value
						throw CustomException("Bad request");
					}
					if (header_value != "chunked")
					{
						error = 400;
						throw CustomException("Bad request");
					}
					transfer_encoding_exists = 1;
					handler2.body_expected = 1;
				}
				if (header_name == "host")
				{
					if (host_exists)
					{
						error = 400;
						throw CustomException("Bad request");
					}
					if (header_value.empty())
					{
						error = 400;
						throw CustomException("Bad request");
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
					// std::cout << "headers fully parsed\n";
					break;
				}
				error = 400;
				throw CustomException("Bad request");
		}
	}

	if (!headers_parsing_done) // is this the correct location to check?
	{
		error = 413; // correct error code when header is too large for buffer OR 431 Request Header Fields Too Large
		throw CustomException("Payload Too Large");  // correct error code when header is too large for buffer
	}
	header_complete = 1;
	body_beginning = handler2.buf_pos; // this is the last ch of the empty line at the end of the headers. Next ch is the first of the body

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
void	Header::checkMethod(RequestHandler& handler)
{
	(void)handler;
	switch (handler2.buf[handler2.buf_pos])
	{
		case 'G':
			// check if next characters are E and T
			if (handler2.buf[++(handler2.buf_pos)] == 'E' && handler2.buf[++(handler2.buf_pos)] == 'T')
				method = "GET";
			else
			{
				error = 501;
				throw CustomException("Not implemented");
			}
			break;
		
		case 'P':
			// check if next characters are O S and T
			if (handler2.buf[++(handler2.buf_pos)] == 'O' && handler2.buf[++(handler2.buf_pos)] == 'S' && handler2.buf[++(handler2.buf_pos)] == 'T')
				method = "POST";
			else
			{
				error = 501;
				throw CustomException("Not implemented");
			}
			break;

		case 'D':
			// check of next characters are E L E T E
			if (handler2.buf[++(handler2.buf_pos)] == 'E' && handler2.buf[++(handler2.buf_pos)] == 'L' && handler2.buf[++(handler2.buf_pos)] == 'E' && handler2.buf[++(handler2.buf_pos)] == 'T' && handler2.buf[++(handler2.buf_pos)] == 'E')
				method = "DELETE";
			else
			{
				error = 501;
				throw CustomException("Not implemented");
			}
			break;
		default:
			error = 501;
			throw CustomException("Not implemented");
			break;
		}
}

// void	Header::checkHttpVersion(unsigned char* buf, int* buf_pos)
void	Header::checkHttpVersion(RequestHandler& handler)
{
	(void)handler;
	if (handler2.buf[handler2.buf_pos] == 'H' && handler2.buf[++(handler2.buf_pos)] == 'T' && handler2.buf[++(handler2.buf_pos)] == 'T' && handler2.buf[++(handler2.buf_pos)] == 'P' && handler2.buf[++(handler2.buf_pos)] == '/' && handler2.buf[++(handler2.buf_pos)] == '1'
		&& handler2.buf[++(handler2.buf_pos)] == '.' && handler2.buf[++(handler2.buf_pos)] == '1')
	{
		version = "HTTP/1.1";
		handler2.buf_pos++;
	}
	else
	{
		error = 505;
		throw CustomException("HTTP Version Not Supported");
	}
}

void	Header::handleMultipleSlashes(RequestHandler& handler) // could probably also be a created as a special state
{
	(void)handler;
	while(handler2.buf[handler2.buf_pos] == '/')
		handler2.buf_pos++;
	handler2.buf_pos--;
}

void	Header::parseRequestLine(RequestHandler& handler)
{
	(void)handler;
	unsigned char	ch;

	// implement max request line rule? (414 and 501 errors)
	// do we need to check for scheme and authority?

	// reserved chars in URI: ! * ' ( ) ; : @ & = + $ , / ? % # [ ]
	// special meanings: + SP / ? % #   & (query)  = (query)  @ (authority)  : (authority)
	// unreseved:  - _ . ~
	// alphanumeric

	rl_state = rl_start; // move to constructor?

	while (!rl_parsing_done && (handler2.buf_pos)++ < handler2.bytes_read)
	{
		ch = handler2.buf[handler2.buf_pos];
	
		switch (rl_state) 
		{
			// in the interest of robustness, ignore at least one empty line
			// double check if a single LF occurance is accepted // is there a space afterwards?
			case rl_start:
				if (handler2.buf_pos == 0 && ch == CR)
					break;
				if (ch == LF)
					break;
				rl_state = rl_method;

			case rl_method: // can this also be termined by CRLF?
				// checkMethod(buf, buf_pos);
				checkMethod(handler2);
				rl_state = rl_first_divider;
				break;

			case rl_first_divider: // may add to method check
				if (ch == SP) // do we want to allow multiple spaces?
					break;
				else if (ch == '/')
				{
					rl_state = rl_path;
					handler2.buf_pos--;
					break;
				}
				error = 400;
				throw CustomException("Bad request");
				
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
						if (handler2.buf[handler2.buf_pos - 1] == '/')
							dot_in_path = 1;
						break;
					case '%':
						path_encoded = 1; // when interpreting request needs to be decoded
						// rl_state = rl_percent_encoded; // need to be followed by numercial code that needs to be interpreted --> otherwise 400 bad request (not the case for query)
						break;
					case '/': // checking with nginx, there can be several slashes after each other
						handleMultipleSlashes(handler2);
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
							error = 400;
							throw CustomException("Bad request");
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
							error = 400;
							throw CustomException("Bad request");
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
					checkHttpVersion(handler2);
				break;

			case rl_cr:
				if (ch == LF)
				{
					rl_state = rl_done;
					break;
				}
				else 
				{
					error = 400;
					throw CustomException("Bad request");
				}
			
			case rl_done:
				std::cout << "request line fully parsed\n";
				rl_parsing_done = 1;
				handler2.buf_pos--; // Why does this needs to be done?
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
		error = 414;
		throw CustomException("Request-URI Too Long");
	}

}
