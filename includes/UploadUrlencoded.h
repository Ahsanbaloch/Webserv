#ifndef UPLOADURLENCODED_H
# define UPLOADURLENCODED_H

# include <map>
# include <fstream>
# include <string>
# include "AUploadModule.h"
# include "RequestHandler.h"
# include "defines.h"
# include "utils.tpp"

class UploadUrlencoded: public AUploadModule
{
private:
	// input
	std::ifstream 						input;
	
	// vars
	int									body_bytes_consumed;
	std::map<std::string, std::string>	database;

	// temp vars
	std::string 						temp_value;
	std::string 						temp_key;
	
	// flags
	bool								encoded_key;
	bool								encoded_value;

	// helper methods
	void								storeInDatabase();
	void								storeInJSON();
	std::string							convertToJSON();
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

	// main method
	void								uploadData();

};


#endif
