
#include "RequestHandler.h"

RequestHandler::RequestHandler(/* args */)
{
}

RequestHandler::~RequestHandler()
{
}

// read request handler
void	RequestHandler::handleRequest(struct kevent event_lst_item)
{
	char buf[BUFFER_SIZE];
	if (event_lst_item.filter == EVFILT_READ)
	{
		int bytes_read = recv(event_lst_item.ident, buf, sizeof(buf), 0);
		if (bytes_read == -1)
			throw CustomException("Failed when processing read request\n");
		printf("read %i bytes\n", bytes_read);	
	}
}
