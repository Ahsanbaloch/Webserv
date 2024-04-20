#ifndef REDIRECTRESPONSE_H
# define REDIRECTRESPONSE_H

# include "AResponse.h"
# include "RequestHandler.h"
# include "Utils.h"

class REDIRECTResponse: public AResponse
{
private:
	// helper methods
	std::string	createHeaderFields();
	std::string	removeSchemeFromURL();

	// construtcors
	REDIRECTResponse();
	REDIRECTResponse(const REDIRECTResponse&);
	REDIRECTResponse& operator=(const REDIRECTResponse&);
	
public:
	explicit REDIRECTResponse(RequestHandler&);
	~REDIRECTResponse();

	// main method
	void	createResponse();
};

#endif
