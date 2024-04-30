
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

	// display success page with link to the newly created resource
	openBodyFile("www/index.html");
	body = createBodyChunk();
	header_fields.append("Location: http://localhost:");
	header_fields.append(toString(handler.getSelectedServer().port));
	header_fields.append(handler.getUploader()->getRelativeFilePath());
	header_fields.append("\r\n");
	header_fields.append("Content-Type: text/html\r\n");
	header_fields.append("Content-Length: ");
	header_fields.append(toString(body_size) + "\r\n");
	header_fields.append("\r\n");
}

///////// MAIN METHOD //////////

void	POSTResponse::createResponse()
{
	handler.setStatus(201);
	respondFileUpload();
}
