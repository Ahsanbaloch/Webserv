
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

	if (access(file.c_str(), F_OK | W_OK) == -1)
	{
		handler.setStatus(404);
		throw CustomException("Not found");
	}
	if (remove(file.c_str()) != 0)
	{
		handler.setStatus(409);
		throw CustomException("Conflict");
	}
	else
		handler.setStatus(204);
}

void	DELETEResponse::deleteDir()
{
	std::string	dir = handler.getLocationConfig().root + handler.getHeaderInfo().getPath();
	
	if (handler.getHeaderInfo().getPath() == "/" || handler.getHeaderInfo().getPath() == "/cgi-bin"
		|| handler.getHeaderInfo().getPath() == "/form_data" || handler.getHeaderInfo().getPath() == "/temp"
		|| handler.getHeaderInfo().getPath() == "/upload" || access(dir.c_str(), F_OK | W_OK) == -1)
	{
		handler.setStatus(404);
		throw CustomException("Not found");
	}
	if (rmdir(dir.c_str()) != 0)
	{
		handler.setStatus(409);
		throw CustomException("Conflict");
	}
	else
		handler.setStatus(204);
}


///////// MAIN METHODS //////////

void	DELETEResponse::createResponse()
{
	// check if file or directory that is requested to be deleted
	if (!handler.getHeaderInfo().getFileExtension().empty())
		deleteFile();
	else
		deleteDir();

	status_line = createStatusLine();
	header_fields = "\r\n\r\n";
}
