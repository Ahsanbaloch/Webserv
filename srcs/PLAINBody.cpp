
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
		unchunkBody(); // inside content type needs to be checked
	else
		; // simply read content into file
		// parseBody();
}
