
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
		size_t bytes_read = recv(event_lst_item.ident, buf, sizeof(buf), 0);
		printf("read %zu bytes\n", bytes_read);	
	}
}