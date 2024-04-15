#ifndef GETRESPONSE_H
# define GETRESPONSE_H

#include <dirent.h> //allowed?
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "AResponse.h"
#include "RequestHandler.h"


class GETResponse: public AResponse
{
private:
	// var
	std::string			file_path; // better name?

	//flags
	bool				auto_index;

	// helper methods
	// std::string	createStatusLine();
	std::string	createBody();
	std::string createHeaderFields(std::string);
	std::string	identifyMIME();
	void		checkInternalRedirects();
	std::string	getBodyFromFile();
	std::string	getBodyFromDir();

	// constructors
	GETResponse();
	GETResponse(const GETResponse&);
	GETResponse& operator=(const GETResponse&);
public:
	// constructors & destructors
	explicit GETResponse(RequestHandler&);
	~GETResponse();

	// main method
	void	createResponse();
};

#endif
