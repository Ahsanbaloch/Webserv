#include "AResponse.h"
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

AResponse::AResponse()
	: handler(*new RequestHandler()), response_complete(0)
{
}

AResponse::AResponse(RequestHandler& request_handler) 
	: handler(request_handler), response_complete(0)
{
}

AResponse::~AResponse()
{
}

AResponse::AResponse(const AResponse& src)
	: handler(src.handler)
{
	file_type = src.file_type;
	full_file_path = src.full_file_path;
	body = src.body;
	status_line = src.status_line;
	header_fields = src.header_fields;
	response_complete = src.response_complete;
}

AResponse& AResponse::operator=(const AResponse& src)
{
	if (this != &src)
	{
		handler = src.handler;
		file_type = src.file_type;
		full_file_path = src.full_file_path;
		body = src.body;
		status_line = src.status_line;
		header_fields = src.header_fields;
		response_complete = src.response_complete;
	}
	return (*this);
}

////////// GETTERS ///////////

std::string AResponse::getRespondsHeaderFields() const
{
	return (header_fields);
}

std::string AResponse::getResponseBody() const
{
	return (body);
}

std::string AResponse::getResponseStatusLine() const
{
	return (status_line);
}

std::string AResponse::getFullFilePath() const
{
	return (full_file_path);
}

bool	AResponse::getResponseCompleteStatus() const
{
	return (response_complete);
}

bool	AResponse::getChunkedBodyStatus() const
{
	return (chunked_body);
}

///////// METHODS ///////////

std::string	AResponse::createStatusLine()
{
	std::string status_line;

	status_line.append("HTTP/1.1 "); // alternative handler.head.version
	status_line.append(toString(handler.getStatus()));
	status_line.append(" \r\n");  //A server MUST send the space that separates the status-code from the reason-phrase even when the reason-phrase is absent (i.e., the status-line would end with the space)
	return (status_line);
}
