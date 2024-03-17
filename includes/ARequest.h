#ifndef AREQUEST_H
# define AREQUEST_H

# include <string>
# include <map>
# include "Response.h"

class RequestHandler;


class ARequest
{
private:
	/* data */
public:
	ARequest(/* args */);
	virtual ~ARequest();

	static ARequest *newRequest(RequestHandler&);

	static void	findServerBlock(RequestHandler&);

	virtual Response *createResponse(RequestHandler&) = 0;

	
};

#endif
