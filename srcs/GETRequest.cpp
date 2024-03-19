
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
	std::string	header;
	std::string mime_type = identifyMIME(handler);
	std::ostringstream length_conversion;
	length_conversion << body.size();

	if (mime_type.empty())
		printf("mime type not supported"); // return some error?
	else
	{
		header.append("Content-Type: " + mime_type + "\r\n");
		header.append("Content-Length: "); // alternatively TE: chunked?
		header.append(length_conversion.str() + "\r\n");
		// header.append("Location: url"); // redirect client to a different url or new path --> will have a specific status code // I assume then there is no body?
		// what other headers to include?
		// send Repsonses in Chunks?
		// header.append("Transfer-Encoding: chunked");
		// header.append("Cache-Control: no-cache");
		// header.append("Set-Cookie: preference=darkmode; Domain=example.com");
		// header.append("Server: nginx/1.21.0");
		// header.append("Expires: Sat, 08 May 2023 12:00:00 GMT"); // If a client requests the same resource before the expiration date has passed, the server can return a cached copy of the resource.
		// header.append("Last-Modified: Tue, 04 May 2023 16:00:00 GMT"); // This header specifies the date and time when the content being sent in the response was last modified. This can be used by clients to determine if the resource has changed since it was last requested.
		// header.append("ETag: "abc123""); //This header provides a unique identifier for the content being sent in the response. This can be used by clients to determine if the resource has changed since it was last requested, without having to download the entire resource again.
		// header.append("Keep-Alive: timeout=5, max=100"); // used to enable persistent connections between the client and the server, allowing multiple requests and responses to be sent over a single TCP connection
		// Access-Control-Allow-Origin; X-Frame-Options; X-XSS-Protection; Referrer-Policy; X-Forwarded-For; X-Powered-By; 
		header.append("\r\n");
	}
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

	// The presence of a message body in a response depends on both the request method to which it is responding and the response status code. 
	// e.g. POST 200 is different from GET 200

	// A server MUST NOT send a Transfer-Encoding header field in any response with a status code of 1xx (Informational) or 204 (No Content)
	// any response with a 1xx (Informational), 204 (No Content), or 304 (Not Modified) status code is always terminated by the first empty line after the header fields --> no body
	return (response);
}


std::string	GETRequest::identifyMIME(RequestHandler& handler)
{
	// how to best identifyMIME?
	if (handler.file_type == "html")
		return ("text/html");
	else if (handler.file_type == "png")
		return ("image/png");
	else
		return (""); // what should be the default return?
}
