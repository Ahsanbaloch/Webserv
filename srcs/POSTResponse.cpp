
#include "POSTResponse.h"

POSTResponse::POSTResponse(RequestHandler& src)
	: AResponse(src)
{
}

POSTResponse::~POSTResponse()
{
}

void	POSTResponse::respondFileUpload()
{
	status_line = createStatusLine();
	header_fields = "Content-Type: plain/text\r\nContent-Length: 3\r\n\r\n"; // maybe send a html response here instead
	body = "201"; // should be gotten from handler.status
	// send redirection?
	// header_fields = ""; // send any header fields?
	// body = "";
}

void	POSTResponse::createResponse()
{
	if (!handler.getLocationConfig().POST)
	{
		handler.setStatus(405);
		throw CustomException("Method Not Allowed");
	}
	else
	{
		// differentiate for different content types?
		handler.setStatus(201);
		respondFileUpload();
	}
}
