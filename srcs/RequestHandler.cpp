
#include "RequestHandler.h"

RequestHandler::RequestHandler(/* args */)
{
	error = 0;
	buf_pos = -1;
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

	while (buf_pos++ < bytes_read)
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

			case rl_method:
				checkMethod();
				if (error == 400)
					throw CustomException("Bad request: method");
				state = rl_first_divider;
				break;

			case rl_first_divider: // may add to method check
				if (ch == SP) // do I want to allow multiple spaces?
					break;
				else if (ch == '/')
				{
					state = rl_path;
					path.append(1, static_cast<char>(ch));
					break;
				}
				error = 400;
				throw CustomException("Bad request: syntax");
				
			case rl_path:
				// do we have to handle authority request lines? Is there always a "host" in the header?
				// query line goes to cgi --> Query string env variable

				switch (ch)
				{
					// do I need to check for CRLF here? (chunked transmission?)
					case SP:
						state = rl_http;
						break;
					case '.': // nginx does not allow two dots at the beginning if nothing comes after; a single dot leads to index (also on some websites with two dots); three dots to file not found
						break;
					case '%':
						complex_path = 1; // when interpreting request needs to be decoded
						state = rl_percent_encoded; // need to be followed by numercial code that needs to be interpreted --> otherwise 400 bad request (not the case for query)
						break;
					case '/': // checking with nginx, there can be several slashes after each other
						break;
					case '?':
						state = rl_query;
						break;
					case '#':
						state = rl_fragment;
					case '+': // means space; needs to be decoded when interpreting request
						complex_path = 1;
						break;
					default:
						if (ch < 32 || ch == 127)
						{
							error = 400;
							throw CustomException("Bad request: syntax");
						}
						break;

				}
				if (state == rl_path)
					path.append(1, static_cast<char>(ch));
				break;

			case rl_percent_encoded: // allowed values #01 - #FF (although ASCII only goes till #7F/7E)
				if (!(loops == 1 && ch == '0') && ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')))
				{
					loops++;
					path.append(1, static_cast<char>(ch));
					if (loops == 2)
						state = rl_path;
					break;
				}
				error = 400;
				throw CustomException("Bad request: syntax");

			case rl_query:
				break;

			case rl_fragment:
				break;

			case rl_http:
				break;
		}

		// why does fragment doesn't add up in buffer when making request with postman?

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
