#ifndef UPLOADPLAIN_H
# define UPLOADPLAIN_H

#include "AUploadModule.h"
#include "RequestHandler.h"
#include "defines.h"

class UploadPlain: public AUploadModule
{
private:
	// input
	std::ifstream				input; // needed?

	// output
	std::ofstream				outfile;

	// vars
	int							body_bytes_consumed;

	// constructors
	UploadPlain();
	UploadPlain(const UploadPlain&);
	UploadPlain& operator=(const UploadPlain&);

public:
	// constructors and desctructors
	explicit UploadPlain(RequestHandler&);
	~UploadPlain();

	// main method
	void	readBody();
};


#endif
