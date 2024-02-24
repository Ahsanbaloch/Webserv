
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

	std::string	method;
	int			error;
	char		buf[BUFFER_SIZE]; // use std::vector<char> buf(BUFFER_SIZE); instead?
	int			buf_pos;

	void	handleRequest(int);
	void	parseRequestLine();
	void	checkMethod();

	enum {
		rl_start = 0,
		rl_method,
		rl_first_divider,
		rl_uri,
		rl_http
	} state;
};

#endif
