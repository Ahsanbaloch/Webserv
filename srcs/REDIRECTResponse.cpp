
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

std::string	REDIRECTResponse::removeSchemeFromURL()
{
	std::string redirect_url;

	std::size_t found_http = handler.getLocationConfig().redirect.find("http://");
	if (found_http == std::string::npos)
	{
		std::size_t found_https = handler.getLocationConfig().redirect.find("https://");
		if (found_https == std::string::npos)
			redirect_url = handler.getLocationConfig().redirect;
		else
			redirect_url.append(handler.getLocationConfig().redirect, 8);
	}
	else
		redirect_url.append(handler.getLocationConfig().redirect, 7);

	return (redirect_url);
}


/////////// MAIN METHODS ///////////

void	REDIRECTResponse::createResponse()
{
	std::string redirect_url = removeSchemeFromURL();

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
