#include "ARequest.h"
#include "GETRequest.h"
#include "RequestHandler.h"

ARequest::ARequest(/* args */)
{
}

ARequest::~ARequest()
{
}


ARequest* ARequest::newRequest(RequestHandler& handler)
{
	// what else to check here?
	// check if the request method is allowed --> can this actually be checked here? because it might depend on the location
		// (alt: compare path and location path already here?)
	
	if (handler.header.method == "GET")
		return (new GETRequest(handler));
	else if (handler.header.method == "DELETE")
		;///
	else if (handler.header.method == "POST")
		;///
	// check if something else and thus not implemented; but currently alsready done when parsing
	return (NULL);
}
