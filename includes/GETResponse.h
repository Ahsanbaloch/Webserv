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
	//vars
	std::ifstream	input_file;
	

	//flags
	bool			auto_index;

	// helper methods
	std::string		getBodyFromDir();
	std::string		createBody();
	std::string		createHeaderFields(std::string);
	std::string		identifyMIME();
	void			determineOutput();

	// constructors
	GETResponse();
	GETResponse(const GETResponse&);
	GETResponse& operator=(const GETResponse&);

public:
	// constructors & destructors
	explicit GETResponse(RequestHandler&);
	~GETResponse();

	// main method
	void			createResponse();
	
};

#endif
