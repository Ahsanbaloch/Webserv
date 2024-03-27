
#include "ERRORResponse.h"

ERRORResponse::ERRORResponse(/* args */)
{
}

ERRORResponse::ERRORResponse(RequestHandler& src)
	:  AResponse(src)
{
}

ERRORResponse::~ERRORResponse()
{
}

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
	std::string default_err_file = "./www/default_error.html";

	// implement check whether location specifies particular error file

	std::ifstream file(default_err_file);
	if (!file.is_open()) 
	{
		body = getDefaultErrorMessage(status_code);
		return (body);
	}
	std::stringstream buffer;
	buffer << file.rdbuf();

	body = buffer.str();
	file.close(); 

	size_t pos = body.find(html_var);
	if (pos != std::string::npos)
		body.replace(pos, html_var.size(), status_code);
	else
	{
		body = getDefaultErrorMessage(status_code);
		return (body);
	}
	return (body);
}

void	ERRORResponse::createResponse()
{
	std::ostringstream status_conversion;
	std::ostringstream length_conversion;

	status_line.append("HTTP/1.1 ");
	status_conversion << handler.getStatus();
	status_line.append(status_conversion.str());
	status_line.append(" \r\n");

	body = createBody(status_conversion.str());
	length_conversion << body.size();

	header_fields.append("Content-Type: text/html\r\n");
	header_fields.append("Content-Length: ");
	header_fields.append(length_conversion.str() + "\r\n");
	header_fields.append("\r\n");
}
