
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
	if (handler.getHeaderInfo().getHeaderFields()["referer"] == handler.getLocationConfig().redirect
		|| handler.getIntRedirRefPath() == handler.getLocationConfig().redirect)
	{
		handler.setStatus(508);
		throw CustomException("Loop Detected");
	}
	else
	{
		handler.setStatus(307);
		status_line = createStatusLine();
		body = "";
		header_fields = createHeaderFields();
	}
}
