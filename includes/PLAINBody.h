#ifndef PLAINBODY_H
# define PLAINBODY_H

#include "ARequestBody.h"
#include "RequestHandler.h"

class PLAINBody: public ARequestBody
{
private:
	// input
	std::ifstream				input; // needed?

	// output
	std::ofstream				outfile;
public:
	// constructors and desctructors
	explicit PLAINBody(RequestHandler&);
	~PLAINBody();

	// main method
	void	readBody();
};


#endif
