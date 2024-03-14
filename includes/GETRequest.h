#ifndef GETREQUEST_H
# define GETREQUEST_H

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
	/* data */
public:
	GETRequest(/* args */);
	explicit GETRequest(RequestHandler&);
	~GETRequest();

	int			is_directory;

	Response	*createResponse(RequestHandler&);
	std::string	createStatusLine(RequestHandler&);
	std::string	createBody(RequestHandler&);
	std::string createHeaderFields(RequestHandler&, std::string);
	void		checkPathType(RequestHandler&);

	std::string	constructFilePath(RequestHandler&);
	void		findDirectory();
};


#endif
