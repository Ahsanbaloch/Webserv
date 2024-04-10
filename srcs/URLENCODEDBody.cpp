
#include "URLENCODEDBody.h"

URLENCODEDBody::URLENCODEDBody(RequestHandler& src)
	: ARequestBody(src)
{
}

URLENCODEDBody::~URLENCODEDBody()
{
}

void	URLENCODEDBody::readBody()
{
	if (handler.getHeaderInfo().getTEStatus())
	{
		unchunkBody(); // inside content type needs to be checked
		// if (handler.body_read)
		// 	parseUnchunkedBody();
	}
	else
		; // implement parsing of body
		// parseBody(); // inside content type needs to be checked
}
