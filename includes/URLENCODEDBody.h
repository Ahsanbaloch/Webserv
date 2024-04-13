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
	std::string 						temp_value;
	std::string 						temp_key;
	bool								encoded_key;
	bool								encoded_value;

	void								storeInDatabase();
	void								decode(std::string&);
public:
	explicit URLENCODEDBody(RequestHandler&);
	~URLENCODEDBody();

	void	readBody();
	void	parseBody(char);

	enum {
		key = 0,
		value,
		end_pair
	} body_state;
};


#endif
