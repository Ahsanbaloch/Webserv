#ifndef REDIRECTRESPONSE_H
# define REDIRECTRESPONSE_H

# include <string>
# include "AResponse.h"
# include "RequestHandler.h"
# include "utils.tpp"

class REDIRECTResponse: public AResponse
{
private:
	// helper methods
	std::string	createHeaderFields();
	
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
