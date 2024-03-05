
#include "RequestHandler.h"

RequestHandler::RequestHandler(/* args */)
{
	error = 0;
	buf_pos = -1;
	rl_parsing_done = 0;
	headers_parsing_done = 0;
	body_parsing_done = 0;
	transfer_encoding_exists = 0;
	content_length_exists = 0;
	method = ""; // does this reset the string?
	path = ""; // does this reset the string?
	query = ""; // does this reset the string?
	version = ""; // does this reset the string?
	memset(&buf, 0, sizeof(buf));
}

RequestHandler::~RequestHandler()
{
}

// read request handler
void	RequestHandler::handleRequest(int event_lst_item_fd)
{	
	bytes_read = recv(event_lst_item_fd, buf, sizeof(buf), 0);
	if (bytes_read == -1)
		throw CustomException("Failed when processing read request\n");
	buf[bytes_read] = '\0'; // correct or bytes_read +1?

	printf("buf: %s\n", buf);

	parseRequestLine();
	parseHeaders();

	printf("\nheaders\n");
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		std::cout << "key: " << it->first << " ";
		std::cout << "value: " << it->second << std::endl;
	}

	if (transfer_encoding_exists && !content_length_exists)
		parseEncodedBody();
	else if (content_length_exists && !transfer_encoding_exists)
		parseContentBody();
	else
	{
		error = 400;
		throw CustomException("Bad request");
		// A server MAY reject a request that contains a message body but not a Content-Length by responding with 411 (Length Required).
	}
	
	std::cout << "identified method: " << method << '\n';
	std::cout << "identified path: " << path << '\n';
	std::cout << "identified query: " << query << '\n';
	std::cout << "identified version: " << version << '\n';

	// check what has been written into body
	std::cout << "identified body: \n";
	std::string line;
	while (std::getline(body, line))
	{
		std::cout << line << std::endl;
	}

	printf("read %i bytes\n", bytes_read);

	// close fd in case bytes_read == 0 ???
}


void	RequestHandler::parseContentBody()
{
	// should be checked as a sequence of octets instead
	body.write(&(buf[buf_pos + 1]), body_length);
}

void	RequestHandler::parseEncodedBody()
{
	// check somewhere that when the transfer encoding contains something different than "chunked" to return an error

	while (!body_parsing_done && buf_pos++ < bytes_read)
	{
		u_char ch = buf[buf_pos];

		te_state = body_start;

		switch (te_state) 
		{
			case body_start:
				// check if body starts with a hex value
				// if yes, move to size

			case chunk_size:
				// get chunk_size
				// go to chunk data reading or extension reading
				// if chunk_size is 0 --> check for chunk trailer

			case chunk_extension:
				// read extension
				// go to chunk data reading

			case chunk_data:
				// read chunks in a for loop using chunk size as a delimiter
				// count data length
				// once read, go back to body start
			
			case chunk_trailer:
				// check if trailer is existing
				// read trailer

			case body_end:
				// termination --> maybe do differently

		}		

	}

		// check chunk-size (first part of body) and translate from hex to integer; followed by CRLF if chunk extension is not provided
			// if chunk-size is 0 and followd by CRLF, the end of the transmission has been reached

		// check for chunk extension, this is followed by CRLF
			// A recipient MUST ignore unrecognized chunk extensions // how to recognize an invalid one?
			// Where to store that extension?

		// run through the chunk data using chunk-size as a delimiter
			// add content to stream until CRLF is reached
			// count data length
			// check again for chunk size and chunk extension

		// if end of data transimission (chunk size 0), check for trailer (finally terminated by empty line --> CRLFCRLF??)
			// store somewhere
			// what info does trailer entail?

	// empty line termination --> always? or only when chunk size == 0?
	
	// set content-length in headers to counted data length --> for what purpose?
	
	


	// The chunked coding does not define any parameters. Their presence SHOULD be treated as an error. --> what is meant by that?



	// A server that receives a request message with a transfer coding it does not understand SHOULD respond with 501 (Not Implemented)
	// This is why Transfer-Encoding is defined as overriding Content-Length, as opposed to them being mutually incompatible.
	// A server MAY reject a request that contains both Content-Length and Transfer-Encoding or process such a request in accordance with the 
	// Transfer-Encoding alone. Regardless, the server MUST close the connection after responding to such a request to avoid the potential attacks.

	// If a valid Content-Length header field is present without Transfer-Encoding, its decimal value defines the expected message body length in octets. 
	//If the sender closes the connection or the recipient times out before the indicated number of octets are received, the recipient MUST 
	// consider the message to be incomplete and close the connection.

	// A recipient MUST ignore unrecognized chunk extensions. A server ought to limit the total length of chunk extensions received in a request 
	// to an amount reasonable for the services provided, in the same way that it applies length limitations and timeouts for other parts of a 
	// message, and generate an appropriate 4xx (Client Error) response if that amount is exceeded

	// use ostringstream to store body?
	
	// chunked-body   = *chunk
                   //last-chunk
                   //trailer-section
                   //CRLF

	buf_pos++;
	printf("I am a body\n");
	printf("buffer pos: %c\n", buf[buf_pos]);
}

void	RequestHandler::checkBodyLength(std::string value)
{
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

void	RequestHandler::parseHeaders()
{
	u_char		ch;
	std::string	header_name = "";
	std::string	header_value = "";

	headers_state = he_start;

	while (!headers_parsing_done && buf_pos++ < bytes_read)
	{
		ch = buf[buf_pos];
	
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
					header_value.append(1, ch);
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
				headers.insert(std::pair<std::string, std::string>(header_name, header_value));
				// check if there is a body in the message
				if (header_name == "content-length")
				{
					content_length_exists = 1;
					checkBodyLength(header_value);
				}
				if (header_name == "transfer-encoding")
					transfer_encoding_exists = 1;
				header_name.clear();
				header_value.clear();
				headers_state = he_start;
				break;

			case hes_end:
				if (ch == LF)
				{
					headers_parsing_done = 1;
					std::cout << "headers fully parsed\n";
					break;
				}
				error = 400;
				throw CustomException("Bad request");
		}
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

void	RequestHandler::checkMethod()
{
	//How to make buf[buf_pos] unsigned char?

	switch (buf[buf_pos])
	{
		case 'G':
			// check if next characters are E and T
			if (buf[++buf_pos] == 'E' && buf[++buf_pos] == 'T')
				method = "GET";
			else
				error = 400;
			break;
		
		case 'P':
			// check if next characters are O S and T
			if (buf[++buf_pos] == 'O' && buf[++buf_pos] == 'S' && buf[++buf_pos] == 'T')
				method = "POST";
			else
				error = 400;
			break;

		case 'D':
			// check of next characters are E L E T E
			if (buf[++buf_pos] == 'E' && buf[++buf_pos] == 'L' && buf[++buf_pos] == 'E' && buf[++buf_pos] == 'T' && buf[++buf_pos] == 'E')
				method = "DELETE";
			else
				error = 400;
			break;
		default:
			// error
			error = 400; // What is the correct error code?
			break;
		}
}

void	RequestHandler::checkHttpVersion()
{
	//How to make buf[buf_pos] unsigned char?

	if (buf[buf_pos] == 'H' && buf[++buf_pos] == 'T' && buf[++buf_pos] == 'T' && buf[++buf_pos] == 'P' && buf[++buf_pos] == '/' && buf[++buf_pos] == '1'
		&& buf[++buf_pos] == '.' && buf[++buf_pos] == '1')
	{
		version = "HTTP/1.1";
		buf_pos++;
	}
	else
		error = 400;
}




void	RequestHandler::parseRequestLine()
{
	u_char	ch;
	int		loops = 0;

	// implement max request line rule? (414 and 501 errors)
	// do we need to check for scheme and authority?

	// Some characters are utilized by URLs for special use in defining their syntax. When these characters are not used in their special role inside a URL, they must be encoded.
	// characters such as {} are getting encoded by the client(?) and being transmitted e.g. with %7B%7D
	// reserved chars in URI: ! * ' ( ) ; : @ & = + $ , / ? % # [ ]
	// special meanings: + SP / ? % #   & (query)  = (query)  @ (authority)  : (authority)
	// unreseved:  - _ . ~
	// alphanumeric

	rl_state = rl_start;

	while (!rl_parsing_done && buf_pos++ < bytes_read)
	{
		ch = buf[buf_pos];
	
		switch (rl_state) 
		{
			// in the interest of robustness, ignore at least one empty line
			// double check if a single LF occurance is accepted // is there a space afterwards?
			case rl_start:
				if (buf_pos == 0 && ch == CR)
					break;
				if (ch == LF)
					break;
				rl_state = rl_method;

			case rl_method: // can this also be termined by CRLF?
				checkMethod();
				if (error == 400)
					throw CustomException("Bad request");
				rl_state = rl_first_divider;
				break;

			case rl_first_divider: // may add to method check
				if (ch == SP) // do we want to allow multiple spaces?
					break;
				else if (ch == '/')
				{
					rl_state = rl_path;
					path.append(1, static_cast<char>(ch));
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
						break;
					case '%':
						path_encoded = 1; // when interpreting request needs to be decoded
						rl_state = rl_percent_encoded; // need to be followed by numercial code that needs to be interpreted --> otherwise 400 bad request (not the case for query)
						break;
					case '/': // checking with nginx, there can be several slashes after each other
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
				if (rl_state == rl_path || rl_state == rl_percent_encoded)
					path.append(1, static_cast<char>(ch));
				break;

			case rl_percent_encoded: // allowed values #01 - #FF (although ASCII only goes till #7F/7E)
				if (!(loops == 1 && ch == '0') && ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')))
				{
					loops++;
					path.append(1, static_cast<char>(ch));
					if (loops % 2 == 0)
						rl_state = rl_path;
					break;
				}
				error = 400;
				throw CustomException("Bad request");

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

			case rl_http: // do we need to check for diffferent versions?
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
				if (error == 400)
					throw CustomException("Bad request");
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
				buf_pos--;
				break;
		}


		// enforce single-space grammar rule? --> check how nginx implements it

		// check for URI (path) --> request target (origin-form)
			// doesn't allow any whitespace
			// if invalid request-line
				// respond with 400 (Bad Request) and close connection
			// shouldn't include the ? + query part (?)

		// enforce single-space grammar rule? --> check how nginx implements it
			// check HTTP version
			// specific error code if not a valid http version (400?)
	}
}
