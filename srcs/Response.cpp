
#include "Response.h"
#include "RequestHandler.h"

Response::Response(/* args */)
{
}

Response::~Response()
{
}

void	Response::errorResponse(RequestHandler& handler)
{
	std::ostringstream status_conversion;
	std::ostringstream length_conversion;

	status_line.append("HTTP/1.1 ");
	status_conversion << handler.header.error;
	status_line.append(status_conversion.str());
	status_line.append(" \r\n");

	body.append(status_conversion.str());
	length_conversion << status_conversion.str().size();

	header_fields.append("Content-Type: plain/text\r\n");
	header_fields.append("Content-Length: ");
	header_fields.append(length_conversion.str() + "\r\n");
	header_fields.append("\r\n");
}
