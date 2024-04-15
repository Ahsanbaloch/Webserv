#ifndef URLENCODEDBODY_H
# define URLENCODEDBODY_H

#include <map>
#include <string>
#include "ARequestBody.h"
#include "RequestHandler.h"

class URLENCODEDBody: public ARequestBody
{
private:
	// input
	std::ifstream 						input;

	// output
	std::map<std::string, std::string>	database;
	
	// vars
	int									body_bytes_consumed;

	// temp vars
	std::string 						temp_value;
	std::string 						temp_key;
	
	// flags
	bool								encoded_key;
	bool								encoded_value;

	// helper methods
	void								storeInDatabase();
	void								decode(std::string&);
	void								parseBody(char);

	// states
	enum {
		key = 0,
		value,
		end_pair
	} body_state;

	// constructors
	URLENCODEDBody();
	URLENCODEDBody(const URLENCODEDBody&);
	URLENCODEDBody& operator=(const URLENCODEDBody&);

public:
	// constructors and desctructors
	explicit URLENCODEDBody(RequestHandler&);
	~URLENCODEDBody();

	// getters
	std::map<std::string, std::string>	getDatabase() const;

	// main method
	void	readBody();

};


#endif
