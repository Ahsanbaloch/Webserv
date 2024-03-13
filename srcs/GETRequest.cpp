
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

Response	*GETRequest::createResponse()
{
	Response *response = new Response; // needs to be delete somewhere

	std::ifstream file("./www/index.html"); // Open the HTML file
    if (!file.is_open()) {
		perror("Failed: ");
        std::cerr << "Error opening file!" << std::endl;
    }

    std::stringstream buffer;
	buffer << file.rdbuf();

	response->body = buffer.str();

    file.close(); // Close the file when done
	
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

