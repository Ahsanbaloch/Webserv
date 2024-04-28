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
	bool								cgi_he_complete;
	
	std::ofstream						outfile;
	std::string							temp_body_filepath;

	enum {
		he_start = 0,
		he_name,
		he_ws,
		he_value,
		he_done,
		he_end
	} cgi_resp_he_state;
	
public:
	CGIResponse(RequestHandler&);
	CGIResponse(/* args */);
	~CGIResponse();

	void	createResponse();
	void	readCGIHeader();
	bool	processBuffer();
	void	readHeaderFields();
	void	storeBody();
	void	createHeaderFields();
	std::string	getTempBodyFilePath();
	
	std::map<std::string, std::string>	cgi_header_fields;


};

#endif
