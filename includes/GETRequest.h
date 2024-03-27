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
	// var
	std::string			file_path; // better name?

	//flags
	bool				auto_index;

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

	void	createResponse();
};

#endif
