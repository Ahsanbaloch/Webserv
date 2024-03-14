
#include "GETRequest.h"

GETRequest::GETRequest(RequestHandler&)
{
	
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

std::string GETRequest::createBody(RequestHandler& handler)
{
	std::string body;

	if (handler.status >= 400) // check if error was identified (or is this handled somewhere else?)
		; // From configData get specific info about which page should be displayed
		// look up file and read content into response body
	else
	{
		std::ifstream file("./www/index.html"); // Open the HTML file
		if (!file.is_open()) {
			perror("Failed: ");
			std::cerr << "Error opening file!" << std::endl;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();

		body = buffer.str();

		file.close(); 
	}
	return (body);
}

std::string	GETRequest::createHeaderFields(RequestHandler& handler)
{
	(void)handler;
	std::string	header;
	std::ostringstream length_conversion;
	length_conversion << handler.response->body.size();

	header.append("Content-Type: text/html\r\n"); // how to determine correct MIME type?
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
	
	// response->header_fields = createHeaderFields(handler);

	// CREATE BODY if required --> seperate function 
	// check if path is a file or a directory (identified by last char in path string)
	// if it is a file
		// find requested file (start looking in X dir?)
	// if it is a dir
		// find directory that should be looked in
		// check if this directory is in the server config
		// if directory is in serverconfig
			// go to directory
			// search for index file specified in config file
				// indexfile found
					// read index file into response body
				// indexfile not found
					// check if auto-index is on
					// if on
						// display directory listing
					// if off
						// provide error reponse
		// if directory is not in serverconfig
			// do something

	
	// CREATE HEADERS
		// if body is sent, calculate size of body (based on data type) and create Content-Length field
			// (should only be sent if TE is not set)
		// How to decide whether to send response in chunks or in one go --> probably based on size of the body
		// Provide content-type field MIME type --> depends on the format of the content being sent, e.g. an html file (Content-Type: text/html)
		// user Location header field for redirection?
		// (set cookie header for cookies)
		// server header --> should this actually be set?
		// additional header fields: expires, last-modified, Access Control Origin, Keep Alive etc.

	// Close the file when done
	
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


	// A sender MUST NOT send whitespace between the start-line and the first header field.

	// The presence of a message body in a response depends on both the request method to which it is responding and the response status code. 
	// e.g. POST 200 is different from GET 200

	// A server MUST NOT send a Transfer-Encoding header field in any response with a status code of 1xx (Informational) or 204 (No Content)
	// any response with a 1xx (Informational), 204 (No Content), or 304 (Not Modified) status code is always terminated by the first empty line after the header fields --> no body

	// check type --> file vs dir
	// check index
	// check auto-index
	return (response);
}

