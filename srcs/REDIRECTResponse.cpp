
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

std::string	REDIRECTResponse::removeSchemeFromURL(std::string input)
{
	std::string redirect_url;

	std::size_t found_http = input.find("http://");
	if (found_http == std::string::npos)
		redirect_url = input;
	else
		redirect_url.append(input, 7);

	return (redirect_url);
}


/////////// MAIN METHODS ///////////

void	REDIRECTResponse::createResponse()
{
	std::string redirect_url = removeSchemeFromURL(handler.getLocationConfig().redirect);

	if (redirect_url == "localhost:" + toString(handler.getServerConfig()[handler.getSelectedLocation()].port) + handler.getLocationConfig().path)
	{
		handler.setStatus(508);
		throw CustomException("Loop Detected");
	}
	else
	{
		handler.setStatus(307);
		status_line = createStatusLine();
		body = ""; // or just initialize it like that // here no body should be created
		header_fields = createHeaderFields();
	}
}
