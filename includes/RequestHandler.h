
#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

#include <sys/socket.h>
#include "CustomException.h"
#include <string>
#include <cstdio> // remove later

#define BUFFER_SIZE 1024
#define LF 10
#define CR 13
#define SP 32

class RequestHandler
{
private:
	/* data */
public:
	RequestHandler(/* args */);
	~RequestHandler();

	void	handleRequest(int);
	void	parseRequestLine(char *buf);
};

#endif
