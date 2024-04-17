
#include "DELETEResponse.h"

///////// CONSTRUCTORS & DESTRUCTOR //////////

DELETEResponse::DELETEResponse()
	: AResponse()
{
}

DELETEResponse::DELETEResponse(RequestHandler& src)
	: AResponse(src)
{
}

DELETEResponse::~DELETEResponse()
{
}

DELETEResponse::DELETEResponse(const DELETEResponse& src)
	: AResponse(src)
{
}

DELETEResponse& DELETEResponse::operator=(const DELETEResponse& src)
{
	if (this != &src)
		AResponse::operator=(src);
	return (*this);
}


///////// HELPER METHODS //////////

void	DELETEResponse::deleteFile()
{
	std::string file = handler.getLocationConfig().root + handler.getHeaderInfo().getPath();

	std::cout << "file to be deleted: " << file << std::endl;

	if (remove(file.c_str()) != 0)
	{
		handler.setStatus(404); // different error code?
		throw CustomException("Not Found");
	}
	else
		handler.setStatus(200);
	
	// send 409 if the file is in use and thus cannot be deleted?
}

void	DELETEResponse::deleteDir()
{
	std::string	dir = handler.getLocationConfig().root + handler.getHeaderInfo().getPath();
	
	std::cout << "directory to be deleted " << dir << std::endl;

	if (handler.getHeaderInfo().getPath() == "/" || rmdir(dir.c_str()) != 0)
	{
		// indicate that it is forbidden to delete directories if they are empty or are the root
		handler.setStatus(403);
		throw CustomException("Forbidden");
	}
	else
		handler.setStatus(200);
}

// std::string	DELETEResponse::createStatusLine() // make Response method? --> set?
// {
// 	std::string status_line;
// 	std::ostringstream status_conversion;

// 	status_line.append("HTTP/1.1 "); // alternative handler.head.version
// 	status_conversion << handler.getStatus();
// 	status_line.append(status_conversion.str());
// 	status_line.append(" \r\n");  //A server MUST send the space that separates the status-code from the reason-phrase even when the reason-phrase is absent (i.e., the status-line would end with the space)
// 	return (status_line);
// }


///////// MAIN METHODS //////////

void	DELETEResponse::createResponse()
{
	// check if file or directory that is requested to be deleted
	if (!handler.getHeaderInfo().getFileExtension().empty())
		deleteFile();
	else
		deleteDir();

	// create extra function for this
	status_line = createStatusLine();
	header_fields = "Content-Type: plain/text\r\nContent-Length: 3\r\n\r\n"; // maybe send a html response here instead
	body = "200"; // should be gotten from handler.status

	// What if there is a deletion request; while a file is in the process of being rendered?

	// send a 202 (Accepted) status code if the action will likely succeed but has not yet been enacted

	// send a 204 (No Content) status code if the action has been enacted and no further information is to be supplied

	// send a 200 (OK) status code if the action has been enacted and the response message includes a representation describing the status
}
