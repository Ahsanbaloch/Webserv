#ifndef URLENCODEDBODY_H
# define URLENCODEDBODY_H

#include "ARequestBody.h"
#include "RequestHandler.h"

class URLENCODEDBody: public ARequestBody
{
private:
	/* data */
public:
	explicit URLENCODEDBody(RequestHandler&);
	~URLENCODEDBody();

	void	readBody();
};


#endif
