
#include "GETRequest.h"

GETRequest::GETRequest(RequestHandler& handler)
{
	
}

GETRequest::GETRequest(/* args */)
{
}

GETRequest::~GETRequest()
{
}

Response	*createResponse()
{
	Response *response = new Response; // needs to be delete somewhere

	
	// check type --> file vs dir
	// check index
	// check auto-index
	return (response);
}

