
#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>
#include <iostream>
#include <string>

#define BUFFER_SIZE 1024

class RequestHandler
{
private:
	/* data */
public:
	RequestHandler(/* args */);
	~RequestHandler();
	void	handleRequest(struct kevent event_lst_item);
};

#endif
