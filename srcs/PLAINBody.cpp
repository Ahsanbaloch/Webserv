
#include "PLAINBody.h"

// PLAINBody::PLAINBody()
// {
// }

PLAINBody::PLAINBody(RequestHandler& src)
	: ARequestBody(src)
{
}

PLAINBody::~PLAINBody()
{
}

void	PLAINBody::readBody()
{
	if (handler.getHeaderInfo().getTEStatus())
	{
		unchunkBody();
		// if (handler.body_read)
		// 	parseUnchunkedBody();
	}
	else
		; // simply read content into file
		// parseBody();
}
