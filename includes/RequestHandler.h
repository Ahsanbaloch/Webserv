
#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

#include <sys/socket.h>
#include "CustomException.h"
#include <string>

#define BUFFER_SIZE 1024

class RequestHandler
{
private:
	/* data */
public:
	RequestHandler(/* args */);
	~RequestHandler();

	void	handleRequest(int);
};

#endif
