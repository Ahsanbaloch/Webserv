
#include "POSTResponse.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

POSTResponse::POSTResponse()
	: AResponse()
{
}

POSTResponse::POSTResponse(RequestHandler& src)
	: AResponse(src)
{
}

POSTResponse::~POSTResponse()
{
}

POSTResponse::POSTResponse(const POSTResponse& src)
	: AResponse(src)
{
}

POSTResponse& POSTResponse::operator=(const POSTResponse& src)
{
	if (this != &src)
		AResponse::operator=(src);
	return (*this);
}

///////// HELPER METHODS //////////

void	POSTResponse::respondFileUpload()
{
	status_line = createStatusLine();

	if (handler.getHeaderInfo().getFileExtension() == ".py")
	{
		body = handler.getCGI()->_cgiOutputStr;
	}
	else
	{
		body = "\n\t\t<!DOCTYPE html>\n\t\t<html lang=\"en\">\n\t\t<head>\n\t\t\t<meta charset=\"UTF-8\">\n\t\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t\t\t<title>Error</title>\n\t\t</head>\n\t";
		body.append("<body><h1>Success</h1></body></html>");
		header_fields.append("Location: http://localhost:");
		header_fields.append(toString(handler.getSelectedServer().port));
		header_fields.append(handler.getUploader()->getRelativeFilePath());
		header_fields.append("\r\n");
	}
	header_fields.append("Content-Type: text/html\r\n");
	header_fields.append("Content-Length: ");
	header_fields.append(toString(body.size()) + "\r\n");
	header_fields.append("\r\n");
}

///////// MAIN METHOD //////////

void	POSTResponse::createResponse()
{
	handler.setStatus(201);
	respondFileUpload();
}
