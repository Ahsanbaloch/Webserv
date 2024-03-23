
#include "DELETERequest.h"

DELETERequest::DELETERequest(RequestHandler&)
{
}

DELETERequest::DELETERequest(/* args */)
{
}

DELETERequest::~DELETERequest()
{
}

Response	*DELETERequest::createResponse(RequestHandler& handler)
{
	(void)handler;
	Response *response = new Response; // needs to be delete somewhere

	// check allowed methods for the selected location
	// if (!handler.server_config[handler.selected_server].locations[handler.selected_location].getAllowed)
		// throw exception

	// if the request is for a folder and valid, contents of the folder need to be delete first

	// What if there is a deletion request; while a file is in the process of being rendered?

	// send a 202 (Accepted) status code if the action will likely succeed but has not yet been enacted

	// send a 204 (No Content) status code if the action has been enacted and no further information is to be supplied

	// send a 200 (OK) status code if the action has been enacted and the response message includes a representation describing the status

	return (response);
	
}
