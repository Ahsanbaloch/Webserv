
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

void	POSTResponse::respondFileUpload()
{
	status_line = createStatusLine();
	// if (handler.content_type == handler.urlencoded)
	// {
	// 	body = createBody(); // tbd
	// 	// set header fields with content type and length
	// }
	// else
	// {

	// }
	header_fields = "Content-Type: plain/text\r\nContent-Length: 3\r\n\r\n";
	header_fields.append("\r\n");
	body = "201"; // should be gotten from handler.status
	// send redirection?
	// header_fields = ""; // send any header fields?
	// body = "";
	
}

// std::string	POSTResponse::createBody()
// {
// 	std::string	body;

// 	return (body);
// }


///////// MAIN METHOD //////////

void	POSTResponse::createResponse()
{
	handler.setStatus(201);
	respondFileUpload();
}
