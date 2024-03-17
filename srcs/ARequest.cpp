#include "ARequest.h"
#include "GETRequest.h"
#include "RequestHandler.h"

ARequest::ARequest(/* args */)
{
}

ARequest::~ARequest()
{
}

void	ARequest::findServerBlock(RequestHandler& handler)
{
	std::vector<t_server_config>::iterator it = handler.server_config.begin();
	for (std::vector<t_server_config>::iterator it2 = handler.server_config.begin(); it2 != handler.server_config.end(); it2++)
	{
		if (it2 == it || it2->serverName == handler.header.header_fields["host"])
			it = it2;
		else
		{
			handler.server_config.erase(it2);
			it2--;
		}
	}
	if (it != handler.server_config.begin())
		handler.server_config.erase(handler.server_config.begin());
}


ARequest* ARequest::newRequest(RequestHandler& handler)
{
	// find server block if there are multiple that match
	if (handler.server_config.size() > 1)
		findServerBlock(handler);

	// what else to check here?
	// check if the request method is allowed --> can this actually be checked here? because it might depend on the location
		// (alt: compare path and location path already here?)
	// some more error handling could go here if not already done in Request Handler (or move it here; e.g. check for http version)
	
	if (handler.header.method == "GET")
		return (new GETRequest(handler));
	else if (handler.header.method == "DELETE")
		;///
	else if (handler.header.method == "POST")
		;///
	// check if something else and thus not implemented; but currently alsready done when parsing
	return (NULL);
}
