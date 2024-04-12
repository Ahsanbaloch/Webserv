
#include "URLENCODEDBody.h"

URLENCODEDBody::URLENCODEDBody(RequestHandler& src)
	: ARequestBody(src)
{
}

URLENCODEDBody::~URLENCODEDBody()
{
}

void	URLENCODEDBody::parseBody()
{
	input.open(filename, std::ios::in);
	// use getline to parse the file; check: "c++ read text file with delimiter"
	// alternatively read byte by byte as the file is small; makes it also easier to identify potential decodings

	// decode body if necessary
}

void	URLENCODEDBody::readBody()
{
	if (handler.getHeaderInfo().getTEStatus())
	{
		unchunkBody();
		if (handler.body_read)
			parseBody();
	}
	else
		; // implement parsing of body
		// parseBody(); // inside content type needs to be checked
}
