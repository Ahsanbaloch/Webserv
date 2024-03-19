
#include "GETRequest.h"

GETRequest::GETRequest(RequestHandler&)
{
	is_directory = 0;
}

GETRequest::GETRequest(/* args */)
{
}

GETRequest::~GETRequest()
{
}

std::string	GETRequest::createStatusLine(RequestHandler& handler)
{
	std::string status_line;
	std::ostringstream err_conversion;

	status_line.append("HTTP/1.1 "); // alternative handler.head.version
	err_conversion << handler.status;
	status_line.append(err_conversion.str());
	status_line.append(" \r\n");  //A server MUST send the space that separates the status-code from the reason-phrase even when the reason-phrase is absent (i.e., the status-line would end with the space)
	return (status_line);
}


void	GETRequest::checkPathType(RequestHandler& handler)
{
	char last_char = handler.header.path[handler.header.path.size() - 1];

	if (last_char == '/')
		is_directory = 1;
	
	std::cout << "is dir: " << is_directory << std::endl;
}

std::string	GETRequest::constructBodyContent(RequestHandler& handler)
{
	std::string body;

	std::ifstream file(handler.file_path); // Open the HTML file
	if (!file.is_open()) 
	{
		// add proper error message
		perror("Failed: ");
		std::cerr << "Error opening file!" << std::endl;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();

	body = buffer.str();

	file.close(); 
	return (body);
}

void	GETRequest::findDirectory()
{
	
}


std::string GETRequest::createBody(RequestHandler& handler)
{
	std::string body;

	if (handler.status >= 400) // check if error was identified (or is this handled somewhere else?)
		; // From configData get specific info about which page should be displayed
		// look up file and read content into response body
	else
		body = constructBodyContent(handler);
	return (body);
}

std::string	GETRequest::createHeaderFields(RequestHandler& handler, std::string body)
{
	(void)handler;
	std::string	header;
	std::string mime_type = identifyMIME(handler);
	std::ostringstream length_conversion;
	length_conversion << body.size();

	if (mime_type.empty())
		printf("mime type not supported"); // return some error? 
	header.append("Content-Type: " + mime_type + "\r\n");
	header.append("Content-Length: ");
	header.append(length_conversion.str());
	header.append("\r\n\r\n");
	return (header);
}

Response	*GETRequest::createResponse(RequestHandler& handler)
{
	Response *response = new Response; // needs to be delete somewhere

	response->status_line = createStatusLine(handler);
	if ((handler.status >= 100 && handler.status <= 103) || handler.status == 204 || handler.status == 304)
		response->body = ""; // or just initialize it like that // here no body should be created
	else
		response->body = createBody(handler);
	
	response->header_fields = createHeaderFields(handler, response->body);


	
	// CREATE HEADERS
		// if body is sent, calculate size of body (based on data type) and create Content-Length field
			// (should only be sent if TE is not set)
		// How to decide whether to send response in chunks or in one go --> probably based on size of the body
		// Provide content-type field MIME type --> depends on the format of the content being sent, e.g. an html file (Content-Type: text/html)
		// user Location header field for redirection?
		// (set cookie header for cookies)
		// server header --> should this actually be set?
		// additional header fields: expires, last-modified, Access Control Origin, Keep Alive etc.

	
	// check if there has been any error detected

	// fill the elements the Response is made of
	// --> What should be the data types? (send takes a const void buf[.len] as argument)
	// --> but can be string.c_str(); so that the response is actually a string
	// a) Status Line = HTTP/1.1 SP <Status Code> SP (<Status Message/reason phrase>) CRLF
		// not necessarily have to send the Status Message as this gets omitted anyway by the client
		// however, the space after the status code has to be sent
	// b) Header fields (no whitespace between field name and colon; one SP after colon) CRLF
		// if body is sent; include content-length or TE
		// how to determine what needs to be sent?
	// c) empty line a.k.a CRLF
	// d) body (optional)



	// The presence of a message body in a response depends on both the request method to which it is responding and the response status code. 
	// e.g. POST 200 is different from GET 200

	// A server MUST NOT send a Transfer-Encoding header field in any response with a status code of 1xx (Informational) or 204 (No Content)
	// any response with a 1xx (Informational), 204 (No Content), or 304 (Not Modified) status code is always terminated by the first empty line after the header fields --> no body
	return (response);
}


std::string	GETRequest::identifyMIME(RequestHandler& handler)
{
	// how to best identifyMIME=
	if (handler.file_type == "html")
		return ("text/html");
	else
		return ("");
}
