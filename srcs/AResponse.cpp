#include "AResponse.h"
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

AResponse::AResponse()
	: handler(*new RequestHandler())
{
}

AResponse::AResponse(RequestHandler& request_handler) 
	: handler(request_handler)
{
}

AResponse::~AResponse()
{
}

AResponse::AResponse(const AResponse& src)
	: handler(src.handler)
{
}

AResponse& AResponse::operator=(const AResponse& src)
{
	if (this != &src)
		handler = src.handler;
	return (*this);
}

////////// GETTERS ///////////

std::string	AResponse::getRedirectedPath() const
{
	return (redirected_path);
}

std::string AResponse::getRespondsHeaderFields() const
{
	return (header_fields);
}

std::string AResponse::getResponseBody() const
{
	return (body);
}

std::string AResponse::getResponseStatusLine() const
{
	return (status_line);
}

///////// METHODS ///////////

std::string	AResponse::createStatusLine() // make Response method? --> set?
{
	std::string status_line;
	std::ostringstream status_conversion;

	status_line.append("HTTP/1.1 "); // alternative handler.head.version
	status_conversion << handler.getStatus();
	status_line.append(status_conversion.str());
	status_line.append(" \r\n");  //A server MUST send the space that separates the status-code from the reason-phrase even when the reason-phrase is absent (i.e., the status-line would end with the space)
	return (status_line);
}


int	AResponse::checkFileExistence()
{	
	if (handler.getLocationConfig().path == "/") // maybe also cases where location ends with /? Is this possible?
		redirected_path = handler.getLocationConfig().root + handler.getLocationConfig().path + handler.getLocationConfig().index;
	else
		redirected_path = handler.getLocationConfig().root + handler.getLocationConfig().path + "/" + handler.getLocationConfig().index;

	std::cout << "index file path: " << redirected_path << std::endl;
	int result = access(redirected_path.c_str(), F_OK); // call to access allowed?
	std::cout << "file exists: " << result << std::endl;
	return (result);
}

bool	AResponse::checkFileType()
{
	// what if there are two dots in the path? // is there a better way to identify the file type requested?
	std::size_t found = handler.getHeaderInfo().getPath().find('.');
	if (found == std::string::npos)
	{
		file_type = "";
		return (0);
	}
	else
	{
		file_type = handler.getHeaderInfo().getPath().substr(found + 1);
		return (1);
	}
}
