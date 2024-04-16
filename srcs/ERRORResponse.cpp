
#include "ERRORResponse.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

ERRORResponse::ERRORResponse()
	: AResponse()
{
}

ERRORResponse::ERRORResponse(RequestHandler& src)
	:  AResponse(src)
{
}

ERRORResponse::~ERRORResponse()
{
}

ERRORResponse::ERRORResponse(const ERRORResponse& src)
	: AResponse(src)
{
}

ERRORResponse& ERRORResponse::operator=(const ERRORResponse& src)
{
	if (this != &src)
	{
		AResponse::operator=(src);
	}
	return (*this);
}


///////// HELPER METHODS //////////

std::string	ERRORResponse::getDefaultErrorMessage(std::string status_code)
{
	std::string body;

	body = "\n\t\t<!DOCTYPE html>\n\t\t<html lang=\"en\">\n\t\t<head>\n\t\t\t<meta charset=\"UTF-8\">\n\t\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t\t\t<title>Error</title>\n\t\t</head>\n\t";
	body.append("<body><h1>" + status_code + "</h1></body></html>");
	return (body);
}

std::string	ERRORResponse::createBody(std::string status_code)
{
	std::string body;
	std::string html_var = "ERROR_CODE";
	std::string err_file = "./www/default_error.html";

	// check if there is a specific error file specified for a partiuclar error code
	// if (handler.getStatus() == handler.getLocationConfig().errorPage.error_page_status)
	// {
	// 	if (handler.getLocationConfig().errorPage.html_page[0] == '/')
	// 		err_file = "." + handler.getLocationConfig().errorPage.html_page;
	// 	else
	// 		err_file = "./" + handler.getLocationConfig().errorPage.html_page;
	// }

	printf("err page path: %s\n", handler.getLocationConfig().errorPage.html_page.c_str());
	printf("err code error_page: %i\n", handler.getLocationConfig().errorPage.error_page_status);

	std::ifstream file(err_file);
	if (!file.is_open()) 
	{
		body = getDefaultErrorMessage(status_code);
		return (body);
	}
	std::stringstream buffer;
	buffer << file.rdbuf();

	body = buffer.str();
	file.close(); 

	// if (handler.getStatus() != handler.getLocationConfig().errorPage.error_page_status)
	// {
	size_t pos = body.find(html_var);
	if (pos != std::string::npos)
		body.replace(pos, html_var.size(), status_code);
	else
	{
		body = getDefaultErrorMessage(status_code);
		return (body);
	}
	// }
	return (body);
}

///////// MAIN METHOD //////////

void	ERRORResponse::createResponse()
{
	std::ostringstream status_conversion;
	std::ostringstream length_conversion;

	status_conversion << handler.getStatus();
	status_line = createStatusLine();
	body = createBody(status_conversion.str());
	length_conversion << body.size();

	header_fields.append("Content-Type: text/html\r\n");
	header_fields.append("Content-Length: ");
	header_fields.append(length_conversion.str() + "\r\n");
	header_fields.append("\r\n");
}
