
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
	std::string path;
	int			error;
	char		buf[BUFFER_SIZE]; // use std::vector<char> buf(BUFFER_SIZE); instead?
	int			buf_pos;
	int			bytes_read;

	void	handleRequest(int);
	void	parseRequestLine();
	void	checkMethod();

	int	complex_path;

	enum {
		rl_start = 0,
		rl_method,
		rl_first_divider,
		rl_path,
		rl_percent_encoded,
		rl_query,
		rl_fragment,
		rl_http
	} state;
};

#endif
