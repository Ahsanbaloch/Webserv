#include "ARequest.h"
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
	
	if (handler.header.method == "GET")
		return (new GETRequest(handler));
	else if (handler.header.method == "DELETE")
		;///
	else if (handler.header.method == "POST")
		;///
	// check if something else and thus not implemented; but currently alsready done when parsing
	
}
