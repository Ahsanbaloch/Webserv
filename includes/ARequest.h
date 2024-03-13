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

	// somewhere create a virtual function and set to 0

	static ARequest *newRequest(RequestHandler&);

	virtual Response *createResponse() = 0;

	
};

#endif