
#include "REDIRECTResponse.h"

/////////// CONSTRUCTORS & DESTRUCTORS ///////////

REDIRECTResponse::REDIRECTResponse()
	: AResponse()
{
}

REDIRECTResponse::REDIRECTResponse(RequestHandler& src)
	: AResponse(src)
{
}

REDIRECTResponse::~REDIRECTResponse()
{
}

REDIRECTResponse::REDIRECTResponse(const REDIRECTResponse& src)
	: AResponse(src)
{
}

REDIRECTResponse& REDIRECTResponse::operator=(const REDIRECTResponse& src)
{
	if (this != &src)
	{
		AResponse::operator=(src);
	}
	return (*this);
}


/////////// HELPER METHODS ///////////

std::string	REDIRECTResponse::createHeaderFields()
{
	std::string	header;

	header.append("Location: " + handler.getLocationConfig().redirect + "\r\n");
	header.append("\r\n");
	return (header);
}


/////////// MAIN METHODS ///////////

void	REDIRECTResponse::createResponse()
{
	handler.setStatus(307);
	status_line = createStatusLine();
	body = ""; // or just initialize it like that // here no body should be created
	header_fields = createHeaderFields();
}
