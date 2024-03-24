
#include "DELETERequest.h"

DELETERequest::DELETERequest(RequestHandler&)
{
}

DELETERequest::DELETERequest(/* args */)
{
}

DELETERequest::~DELETERequest()
{
}

void	DELETERequest::deleteFile(RequestHandler& handler)
{
	std::string file = handler.server_config[handler.selected_server].locations[handler.selected_location].root + handler.header.path;

	std::cout << "fileeee: " << file << std::endl;

	if (remove(file.c_str()) != 0)
	{
		handler.header.error = 404; // different error code?
		throw CustomException("Not Found");
	}
	else
		handler.status = 200;
	
	// send 409 if the file is in use and thus cannot be deleted?
}

void	DELETERequest::deleteDir(RequestHandler& handler)
{
	std::string	dir = handler.server_config[handler.selected_server].locations[handler.selected_location].root + handler.header.path;
	
	std::cout << "dirrrr: " << dir << std::endl;

	if (handler.header.path == "/" || rmdir(dir.c_str()) != 0)
	{
		// indicate that it is forbidden to delete directories if they are empty or are the root
		handler.header.error = 403;
		throw CustomException("Forbidden");
	}
	else
		handler.status = 200;
}


std::string	DELETERequest::createStatusLine(RequestHandler& handler) // make Response method? --> set?
{
	std::string status_line;
	std::ostringstream status_conversion;

	status_line.append("HTTP/1.1 "); // alternative handler.head.version
	status_conversion << handler.status;
	status_line.append(status_conversion.str());
	status_line.append(" \r\n");  //A server MUST send the space that separates the status-code from the reason-phrase even when the reason-phrase is absent (i.e., the status-line would end with the space)
	return (status_line);
}



Response	*DELETERequest::createResponse(RequestHandler& handler)
{
	Response *response = new Response; // needs to be delete somewhere

	// check allowed methods for the selected location
	// if (!handler.server_config[handler.selected_server].locations[handler.selected_location].getAllowed)
		// throw exception

	// check if file or directory that is requested to be deleted
	if (checkFileType(handler))
		deleteFile(handler);
	else
		deleteDir(handler);

	response->status_line = createStatusLine(handler);
	response->header_fields = "Content-Type: plain/text\r\nContent-Length: 3\r\n\r\n"; // maybe send a html response here instead
	response->body = "200"; // should be gotten from handler.status


	// What if there is a deletion request; while a file is in the process of being rendered?

	// send a 202 (Accepted) status code if the action will likely succeed but has not yet been enacted

	// send a 204 (No Content) status code if the action has been enacted and no further information is to be supplied

	// send a 200 (OK) status code if the action has been enacted and the response message includes a representation describing the status

	return (response);
	
}
