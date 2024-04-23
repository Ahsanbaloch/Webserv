#ifndef GETRESPONSE_H
# define GETRESPONSE_H

# include <dirent.h>
# include <iostream>
# include <fstream>
# include <string>
# include "AResponse.h"
# include "RequestHandler.h"
# include "utils.tpp"

class GETResponse: public AResponse
{
private:
	//flags
	bool		auto_index;

	std::ifstream file;
	std::streampos file_position;

	// helper methods
	
	std::string	getBodyFromDir();
	std::string	createBody();
	std::string	createHeaderFields(std::string);
	std::string	identifyMIME();
	void		determineOutput();

	// constructors
	GETResponse();
	GETResponse(const GETResponse&);
	GETResponse& operator=(const GETResponse&);

public:
	// constructors & destructors
	explicit GETResponse(RequestHandler&);
	~GETResponse();

	// main method
	void		createResponse();
	std::string	getBodyFromFile();
	
};

#endif
