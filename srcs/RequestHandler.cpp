
#include "RequestHandler.h"

RequestHandler::RequestHandler(/* args */)
{
}

RequestHandler::~RequestHandler()
{
}

// read request handler
void	RequestHandler::handleRequest(int event_lst_item_fd)
{
	char buf[BUFFER_SIZE]; // use std::vector<char> buf(BUFFER_SIZE); instead?
	
	int bytes_read = recv(event_lst_item_fd, buf, sizeof(buf), 0);
	if (bytes_read == -1)
		throw CustomException("Failed when processing read request\n");
	buf[bytes_read] = '\0';
	printf("buf: %s\n", buf);
	printf("read %i bytes\n", bytes_read);	

	// close fd in case bytes_read == 0 ???
}

void	RequestHandler::parseRequestLine(char *buf)
{
	int j = 0;

	// implement max request line rule? (414 and 501 errors)

	// in the interest of robustness, ignore at least one empty line
	// double check if a single LF occurance is accepted
	if (j == 0 && ((buf[j] == CR && buf[j + 1] == LF)) || buf[j] == LF)
	{
		if (buf[j] == LF)
			j++;
		else
			j += 2;
	}

	for (int i = j; i < BUFFER_SIZE; i++)
	{
		// check for method
		switch (buf[i])
		{
		case 'G'
			// check if next characters are E and T
			break;
		
		case 'P'
			// check if next characters are O S and T
			break;

		case 'D'
			// check of next characters are E L E T E

		default:
			// error
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
