#ifndef URLENCODEDBODY_H
# define URLENCODEDBODY_H

#include <map>
#include <string>
#include "ARequestBody.h"
#include "RequestHandler.h"

class URLENCODEDBody: public ARequestBody
{
private:
	std::map<std::string, std::string>	database;
	std::ifstream 						input;
public:
	explicit URLENCODEDBody(RequestHandler&);
	~URLENCODEDBody();

	void	readBody();
	void	parseBody();
};


#endif
