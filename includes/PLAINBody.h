#ifndef PLAINBODY_H
# define PLAINBODY_H

#include "ARequestBody.h"
#include "RequestHandler.h"

class PLAINBody: public ARequestBody
{
private:
	/* data */
public:
	// PLAINBody(/* args */);
	explicit PLAINBody(RequestHandler&);
	~PLAINBody();

	void	readBody();
};


#endif
