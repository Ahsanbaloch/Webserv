
#include "RequestHandler.h"

RequestHandler::RequestHandler(/* args */)
{
	error = 0;
	buf_pos = -1;
	parsing_done = 0;
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
	// char buf[BUFFER_SIZE]; // use std::vector<char> buf(BUFFER_SIZE); instead?
	
	bytes_read = recv(event_lst_item_fd, buf, sizeof(buf), 0);
	if (bytes_read == -1)
		throw CustomException("Failed when processing read request\n");
	buf[bytes_read] = '\0'; // correct or bytes_read +1?
	
	printf("buf: %s\n\n", buf);
	parseRequestLine();

	std::cout << "identified method: " << method << '\n';
	std::cout << "identified path: " << path << '\n';
	std::cout << "identified query: " << query << '\n';
	std::cout << "identified version: " << version << '\n';


	printf("read %i bytes\n", bytes_read);	

	// close fd in case bytes_read == 0 ???
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

	state = rl_start;

	while (buf_pos++ < bytes_read && !parsing_done)
	{
		ch = buf[buf_pos];
	
		switch (state) 
		{
			// in the interest of robustness, ignore at least one empty line
			// double check if a single LF occurance is accepted // is there a space afterwards?
			case rl_start:
				if (buf_pos == 0 && ch == CR)
					break;
				if (ch == LF)
					break;
				state = rl_method;

			case rl_method: // can this also be termined by CRLF?
				checkMethod();
				if (error == 400)
					throw CustomException("Bad request");
				state = rl_first_divider;
				break;

			case rl_first_divider: // may add to method check
				if (ch == SP) // do we want to allow multiple spaces?
					break;
				else if (ch == '/')
				{
					state = rl_path;
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
						state = rl_http;
						break;
					case CR: // what if this occurs and no http version is specified?
						state = rl_cr;
						break;
					case LF: // what if this occurs and no http version is specified?
						state = rl_done;
						break;
					case '.': // nginx does not allow two dots at the beginning if nothing comes after; a single dot leads to index (also on some websites with two dots); three dots to file not found
						break;
					case '%':
						path_encoded = 1; // when interpreting request needs to be decoded
						state = rl_percent_encoded; // need to be followed by numercial code that needs to be interpreted --> otherwise 400 bad request (not the case for query)
						break;
					case '/': // checking with nginx, there can be several slashes after each other
						break;
					case '?':
						state = rl_query;
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
				if (state == rl_path || state == rl_percent_encoded)
					path.append(1, static_cast<char>(ch));
				break;

			case rl_percent_encoded: // allowed values #01 - #FF (although ASCII only goes till #7F/7E)
				if (!(loops == 1 && ch == '0') && ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')))
				{
					loops++;
					path.append(1, static_cast<char>(ch));
					if (loops % 2 == 0)
						state = rl_path;
					break;
				}
				error = 400;
				throw CustomException("Bad request");

			case rl_query:
				switch (ch)
				{
					case SP:
						state = rl_http;
						break;
					case CR:
						state = rl_cr;
						break;
					case LF:
						state = rl_done;
						break;
					default:
						if (ch < 32 || ch == 127)
						{
							error = 400;
							throw CustomException("Bad request");
						}
						break;
				}
				if (state == rl_query)
					query.append(1, static_cast<char>(ch));
				break;

			case rl_http: // do we need to check for diffferent versions?
				if (ch == CR)
				{
					state = rl_cr;
					break;
				}
				if (ch == LF)
				{
					state = rl_done;
					break;
				}
				if (ch == 'H')
					checkHttpVersion();
				if (error == 400)
					throw CustomException("Bad request");
				break;

			case rl_cr:
				if (ch == LF)
					state = rl_done;
				break;
				error = 400;
				throw CustomException("Bad request");
			
			case rl_done:
				std::cout << "request line fully parsed\n";
				parsing_done = 1;
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
