#ifndef PLAINBODY_H
# define PLAINBODY_H

#include "ARequestBody.h"
#include "RequestHandler.h"

class PLAINBody: public ARequestBody
{
private:
	std::ifstream				input;
	std::ofstream				temp;
public:
	// PLAINBody(/* args */);
	explicit PLAINBody(RequestHandler&);
	~PLAINBody();

	void	readBody();
};


#endif
