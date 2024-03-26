#ifndef DELETEREQUEST_H
# define DELETEREQUEST_H

#include <iostream>
#include <sstream>
#include <cstdio>
#include "ARequest.h"
#include "RequestHandler.h"
#include "Response.h"

class DELETERequest: public ARequest
{
private:
	/* data */
public:
	DELETERequest();
	explicit DELETERequest(RequestHandler&);
	~DELETERequest();

	Response	*createResponse(RequestHandler&);

	void		deleteFile(RequestHandler&);
	void		deleteDir(RequestHandler&);
	std::string	createStatusLine(RequestHandler&);
};



#endif
