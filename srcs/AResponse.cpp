#include "AResponse.h"
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

AResponse::AResponse()
	: handler(*new RequestHandler())
{
	internal_redirect = 0;
}

AResponse::AResponse(RequestHandler& request_handler) 
	: handler(request_handler)
{
	internal_redirect = 0;
}

AResponse::~AResponse()
{
}

AResponse::AResponse(const AResponse& src)
	: handler(src.handler)
{
	file_type = src.file_type;
	redirected_path = src.redirected_path;
	body = src.body;
	status_line = src.status_line;
	header_fields = src.header_fields;
	internal_redirect = src.internal_redirect;
}

AResponse& AResponse::operator=(const AResponse& src)
{
	if (this != &src)
	{
		handler = src.handler;
		file_type = src.file_type;
		redirected_path = src.redirected_path;
		body = src.body;
		status_line = src.status_line;
		header_fields = src.header_fields;
		internal_redirect = src.internal_redirect;
	}
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

bool	AResponse::getInternalRedirectStatus() const
{
	return (internal_redirect);
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

void	AResponse::identifyRedirectedPath()
{
	if (!handler.getLocationConfig().path.empty() && handler.getLocationConfig().path[handler.getLocationConfig().path.length() - 1] == '/')
	{
		if (!handler.getLocationConfig().index.empty() && handler.getLocationConfig().index[0] != '/')
			redirected_path = handler.getLocationConfig().root + handler.getLocationConfig().path + handler.getLocationConfig().index;
		else
			redirected_path = handler.getLocationConfig().root + handler.getLocationConfig().path + handler.getLocationConfig().index.substr(1);
	}
	else
	{
		if (!handler.getLocationConfig().index.empty() && handler.getLocationConfig().index[0] != '/')
			redirected_path = handler.getLocationConfig().root + handler.getLocationConfig().path + "/" + handler.getLocationConfig().index;
		else
			redirected_path = handler.getLocationConfig().root + handler.getLocationConfig().path + handler.getLocationConfig().index;
	}
	std::cout << "index file path: " << redirected_path << std::endl;
}

int	AResponse::checkFileExistence(std::string path)
{	
	int result = access(path.c_str(), F_OK);
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
