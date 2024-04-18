#ifndef PLAINBODY_H
# define PLAINBODY_H

#include "AUploadModule.h"
#include "RequestHandler.h"
#include "defines.h"

class PLAINBody: public AUploadModule
{
private:
	// input
	std::ifstream				input; // needed?

	// output
	std::ofstream				outfile;

	// vars
	int							body_bytes_consumed;

	// constructors
	PLAINBody();
	PLAINBody(const PLAINBody&);
	PLAINBody& operator=(const PLAINBody&);

public:
	// constructors and desctructors
	explicit PLAINBody(RequestHandler&);
	~PLAINBody();

	// main method
	void	readBody();
};


#endif
