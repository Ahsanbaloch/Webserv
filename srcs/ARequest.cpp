#include "ARequest.h"
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

ARequest::ARequest()
	: handler(*new RequestHandler())
{
}

ARequest::ARequest(RequestHandler& request_handler) 
	: handler(request_handler)
{
}

ARequest::~ARequest()
{
}

ARequest::ARequest(const ARequest& src)
	: handler(src.handler)
{
}

ARequest& ARequest::operator=(const ARequest& src)
{
	if (this != &src)
		handler = src.handler;
	return (*this);
}

///////// METHODS ///////////

int	ARequest::checkFileExistence()
{	
	if (handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].path == "/") // maybe also cases where location ends with /? Is this possible?
		handler.header.redirected_path = handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].root + handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].path + handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].index;
	else
		handler.header.redirected_path = handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].root + handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].path + "/" + handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].index;

	std::cout << "index file path: " << handler.header.redirected_path << std::endl;
	int result = access(handler.header.redirected_path.c_str(), F_OK); // call to access allowed?
	std::cout << "file exists: " << result << std::endl;
	return (result);
}

bool	ARequest::checkFileType()
{
	// what if there are two dots in the path? // is there a better way to identify the file type requested?
	std::size_t found = handler.header.getPath().find('.');
	if (found == std::string::npos)
	{
		handler.file_type = "";
		return (0);
	}
	else
	{
		handler.file_type = handler.header.getPath().substr(found + 1);
		return (1);
	}
}
