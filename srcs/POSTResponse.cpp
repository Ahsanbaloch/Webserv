
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
	header_fields = "Content-Type: plain/text\r\nContent-Length: 3\r\n\r\n"; // maybe send a html response here instead
	body = "201"; // should be gotten from handler.status
	// send redirection?
	// header_fields = ""; // send any header fields?
	// body = "";
}


///////// MAIN METHOD //////////

void	POSTResponse::createResponse()
{
	handler.setStatus(201);
	respondFileUpload();
}
