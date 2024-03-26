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
	// helper methods
	void		deleteFile();
	void		deleteDir();
	std::string	createStatusLine();

	// constructors
	DELETERequest();
	DELETERequest(const DELETERequest&);
	DELETERequest& operator=(const DELETERequest&);

public:
	// constructors & destructors
	explicit DELETERequest(RequestHandler&);
	~DELETERequest();

	Response	*createResponse();

};

#endif
