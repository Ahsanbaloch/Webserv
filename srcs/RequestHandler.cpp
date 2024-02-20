
#include "../includes/RequestHandler.h"

RequestHandler::RequestHandler(/* args */)
{
}

RequestHandler::~RequestHandler()
{
}

// read request handler
void	RequestHandler::handleRequest(int event_lst_item_fd)
{
	char buf[BUFFER_SIZE];
	
	int bytes_read = recv(event_lst_item_fd, buf, sizeof(buf), 0);
	if (bytes_read == -1)
		throw CustomException("Failed when processing read request\n");
	printf("read %i bytes\n", bytes_read);	

	// close fd in case bytes_read == 0 ???
}

