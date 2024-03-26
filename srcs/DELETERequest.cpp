
#include "DELETERequest.h"

///////// CONSTRUCTORS & DESTRUCTOR //////////

DELETERequest::DELETERequest(RequestHandler& src)
	: ARequest(src)
{
}

DELETERequest::DELETERequest(/* args */)
	: ARequest()
{
}

DELETERequest::~DELETERequest()
{
}

DELETERequest::DELETERequest(const DELETERequest& src)
	: ARequest(src)
{
}

DELETERequest& DELETERequest::operator=(const DELETERequest& src)
{
	if (this != &src)
		ARequest::operator=(src);
	return (*this);
}


///////// HELPER METHODS //////////

void	DELETERequest::deleteFile()
{
	std::string file = handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].root + handler.header.getPath();

	std::cout << "fileeee: " << file << std::endl;

	if (remove(file.c_str()) != 0)
	{
		handler.status = 404; // different error code?
		throw CustomException("Not Found");
	}
	else
		handler.status = 200;
	
	// send 409 if the file is in use and thus cannot be deleted?
}

void	DELETERequest::deleteDir()
{
	std::string	dir = handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].root + handler.header.getPath();
	
	std::cout << "dirrrr: " << dir << std::endl;

	if (handler.header.getPath() == "/" || rmdir(dir.c_str()) != 0)
	{
		// indicate that it is forbidden to delete directories if they are empty or are the root
		handler.status = 403;
		throw CustomException("Forbidden");
	}
	else
		handler.status = 200;
}

std::string	DELETERequest::createStatusLine() // make Response method? --> set?
{
	std::string status_line;
	std::ostringstream status_conversion;

	status_line.append("HTTP/1.1 "); // alternative handler.head.version
	status_conversion << handler.status;
	status_line.append(status_conversion.str());
	status_line.append(" \r\n");  //A server MUST send the space that separates the status-code from the reason-phrase even when the reason-phrase is absent (i.e., the status-line would end with the space)
	return (status_line);
}


///////// MAIN METHODS //////////

Response	*DELETERequest::createResponse()
{
	Response *response = new Response; // needs to be delete somewhere

	// check allowed methods for the selected location
	// if (!handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].getAllowed)
		// throw exception

	// check if file or directory that is requested to be deleted
	if (checkFileType())
		deleteFile();
	else
		deleteDir();

	response->status_line = createStatusLine();
	response->header_fields = "Content-Type: plain/text\r\nContent-Length: 3\r\n\r\n"; // maybe send a html response here instead
	response->body = "200"; // should be gotten from handler.status


	// What if there is a deletion request; while a file is in the process of being rendered?

	// send a 202 (Accepted) status code if the action will likely succeed but has not yet been enacted

	// send a 204 (No Content) status code if the action has been enacted and no further information is to be supplied

	// send a 200 (OK) status code if the action has been enacted and the response message includes a representation describing the status

	return (response);
	
}
