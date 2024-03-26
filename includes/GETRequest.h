#ifndef GETREQUEST_H
# define GETREQUEST_H

#include <dirent.h> //allowed?
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "ARequest.h"
#include "RequestHandler.h"
#include "Response.h"


class GETRequest: public ARequest
{
private:
	// helper methods
	std::string	createStatusLine();
	std::string	createBody();
	std::string createHeaderFields(std::string);
	std::string	identifyMIME();
	void		checkRedirects();
	std::string	getBodyFromFile();
	std::string	getBodyFromDir();

	// constructors
	GETRequest();
	GETRequest(const GETRequest&);
	GETRequest& operator=(const GETRequest&);
public:
	// constructors & destructors
	explicit GETRequest(RequestHandler&);
	~GETRequest();

	Response	*createResponse();
};

#endif
