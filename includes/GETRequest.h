#ifndef GETREQUEST_H
# define GETREQUEST_H

#include "ARequest.h"
#include "RequestHandler.h"
#include "Response.h"

class GETRequest: public ARequest
{
private:
	/* data */
public:
	GETRequest(/* args */);
	explicit GETRequest(RequestHandler&);
	~GETRequest();

	Response	*createResponse();
};


#endif