
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
	if (handler.getStatus() == handler.getLocationConfig().errorPage.error_page_status)
		err_file = handler.getLocationConfig().errorPage.html_page;

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

	if (handler.getStatus() != handler.getLocationConfig().errorPage.error_page_status)
	{
		size_t pos = body.find(html_var);
		if (pos != std::string::npos)
			body.replace(pos, html_var.size(), status_code);
		else
		{
			body = getDefaultErrorMessage(status_code);
			return (body);
		}
	}
	return (body);
}

void	ERRORResponse::appendAllowedMethods()
{
	std::vector<std::string> methods;

	if (handler.getLocationConfig().GET)
		methods.push_back("GET");
	if (handler.getLocationConfig().POST)
		methods.push_back("POST");
	if (handler.getLocationConfig().DELETE)
		methods.push_back("DELETE");

	header_fields.append("Allow: ");
	for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it)
	{
		if (it != methods.begin())
			header_fields.append(", ");
		header_fields.append(*it);
	}
	header_fields.append("\r\n");
}


///////// MAIN METHOD //////////

void	ERRORResponse::createResponse()
{
	status_line = createStatusLine();
	body = createBody(toString(handler.getStatus()));
	header_fields.append("Content-Type: text/html\r\n");
	header_fields.append("Content-Length: ");
	header_fields.append(toString(body.size()) + "\r\n");
	if (handler.getStatus() == 405)
		appendAllowedMethods();
	header_fields.append("\r\n");
}
