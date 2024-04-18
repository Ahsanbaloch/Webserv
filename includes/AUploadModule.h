#ifndef AUPLOADMODULE_H
# define AUPLOADMODULE_H

#include <string>
#include <map>
#include <fstream>
#include "defines.h"

class RequestHandler;

class AUploadModule
{
protected:
	RequestHandler&	handler;

	// vars
	std::string			filename; // move to children? (if not needed in all sub-classes)

	// flags
	bool			body_read;
	bool			body_parsing_done; // only needed in Multipart class

	// constructors
	AUploadModule();
	AUploadModule(const AUploadModule&);
	AUploadModule& operator=(const AUploadModule&);

public:
	// constructors & destructors
	explicit AUploadModule(RequestHandler&);
	virtual ~AUploadModule();

	// getters
	bool			getBodyProcessed() const;

	// methods
	virtual void	readBody() = 0;

};

#endif
