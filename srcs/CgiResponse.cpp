
#include "CGIResponse.h"

CGIResponse::CGIResponse(RequestHandler& src)
	: AResponse(src)
{

}

CGIResponse::CGIResponse(/* args */)
	: AResponse()
{
}

CGIResponse::~CGIResponse()
{
}


void	CGIResponse::createResponse()
{
	// need to read the header line of the CGI response
	// needs to be at least one line seperated by newline
	// may include status code seperated by space
		
		// Content-Type --> if html has been created (e.g. form)
		// Location --> local resource --> indicates to server to reprocess request with the new path
			// if internal redircect --> reset path, filename and file extension and reseach for location
			// find new Location
			// make getResponse

	// if (handler.getHeaderInfo().getMethod() == "GET")
	// {

	// }
	// else
	// {
	// 	handler.setStatus(200);

	// }
}
