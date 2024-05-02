#ifndef CGIRESPONSE_H
# define CGIRESPONSE_H

# include <vector>
# include <string>
# include <map>
# include "RequestHandler.h"
# include "AResponse.h"
# include "CGIHandler.h"
# include "utils.h"
# include "utils.tpp"

class CGIResponse: public AResponse
{
private:
	//vars
	std::map<std::string, std::string>	cgi_header_fields;
	std::vector<std::string> 			valid_headers;
	std::ofstream						outfile;
	std::string							temp_body_filepath;

	// flags
	bool								cgi_he_complete;
	
	// states
	enum {
		he_start = 0,
		he_name,
		he_ws,
		he_value,
		he_done,
		he_end
	} cgi_resp_he_state;

	// helper methods
	void								readHeaderFields();
	void								storeBody();
	void								createHeaderFields();
	std::vector<std::string>			getValidHTTPHeaders();
	
	// constructors
	CGIResponse();
	CGIResponse(const CGIResponse&);
	CGIResponse& operator=(const CGIResponse&);

public:
	// constructors & desctructors
	explicit CGIResponse(RequestHandler&);
	
	~CGIResponse();

	// getters
	std::string							getTempBodyFilePath() const;
	std::map<std::string, std::string>	getCGIHeaderFields() const;

	// main method
	void								createResponse();
	bool								processBuffer();

};

#endif
