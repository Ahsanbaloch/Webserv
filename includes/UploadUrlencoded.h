#ifndef UPLOADURLENCODED_H
# define UPLOADURLENCODED_H

#include <map>
#include <string>
#include "AUploadModule.h"
#include "RequestHandler.h"

class UploadUrlencoded: public AUploadModule
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
	UploadUrlencoded();
	UploadUrlencoded(const UploadUrlencoded&);
	UploadUrlencoded& operator=(const UploadUrlencoded&);

public:
	// constructors and desctructors
	explicit UploadUrlencoded(RequestHandler&);
	~UploadUrlencoded();

	// getters
	std::map<std::string, std::string>	getDatabase() const;

	// main method
	void	readBody();

};


#endif
