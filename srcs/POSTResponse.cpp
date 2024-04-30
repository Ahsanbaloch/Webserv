
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

std::string	POSTResponse::createBody()
{
	std::string	temp_body;
	std::string placeholder_link = "{resource_link}";
	std::string resource_link = "http://localhost:" + toString(handler.getSelectedServer().port) + handler.getUploader()->getRelativeFilePath();

	temp_body = readHTMLTemplateFile("www/success_template.html");
	size_t pos = temp_body.find(placeholder_link);
	if (pos != std::string::npos)
		temp_body.replace(pos, placeholder_link.length(), resource_link);
	return (temp_body);
}

void	POSTResponse::respondFileUpload()
{
	status_line = createStatusLine();
	body = createBody();
	header_fields.append("Location: http://localhost:");
	header_fields.append(toString(handler.getSelectedServer().port));
	header_fields.append(handler.getUploader()->getRelativeFilePath());
	header_fields.append("\r\n");
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
