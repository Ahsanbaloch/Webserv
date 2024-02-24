
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
	
	int bytes_read = recv(event_lst_item_fd, buf, sizeof(buf), 0);
	if (bytes_read == -1)
		throw CustomException("Failed when processing read request\n");
	buf[bytes_read] = '\0'; // correct or bytes_read +1?
	
	printf("buf: %s\n\n", buf);
	parseRequestLine();

	std::cout << "identified method: " << method << '\n';

	printf("read %i bytes\n", bytes_read);	

	// close fd in case bytes_read == 0 ???
}

void	RequestHandler::checkMethod()
{
	switch (buf[buf_pos])
	{
		case 'G':
			// check if next characters are E and T
			if (buf[buf_pos + 1] == 'E' && buf[buf_pos + 2] == 'T')
			{
				method = "GET";
				buf_pos += 2;
			}
			else
				error = 400;
			break;
		
		case 'P':
			// check if next characters are O S and T
			if (buf[buf_pos + 1] == 'O' && buf[buf_pos + 2] == 'S' && buf[buf_pos + 3] == 'T')
			{
				method = "POST";
				buf_pos += 3;
			}
			else
				error = 400;
			break;

		case 'D':
			// check of next characters are E L E T E
			if (buf[buf_pos + 1] == 'E' && buf[buf_pos + 2] == 'L' && buf[buf_pos + 3] == 'E' && buf[buf_pos + 4] == 'T' && buf[buf_pos + 5] == 'E')
			{
				method = "DELETE";
				buf_pos += 5;
			}
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
	u_char ch;

	// implement max request line rule? (414 and 501 errors)

	state = rl_start;

	while (buf_pos++ < BUFFER_SIZE)
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

			case rl_first_divider:
				if (ch == SP)
				{
					state = rl_uri;
					break;
				}
				error = 400;
				throw CustomException("Bad request: syntax");
				
			case rl_uri:
				
				break;

			case rl_http:
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
