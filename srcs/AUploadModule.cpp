
#include "AUploadModule.h"
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

AUploadModule::AUploadModule()
	: handler(*new RequestHandler())
{
	body_read = 0;
	body_parsing_done = 0;
}

AUploadModule::AUploadModule(RequestHandler& src)
	: handler(src)
{
	body_read = 0;
	body_parsing_done = 0;
}

AUploadModule::~AUploadModule()
{
}

AUploadModule::AUploadModule(const AUploadModule& src)
	: handler(src.handler)
{
	body_read = src.body_read;
	body_parsing_done = src.body_parsing_done;
	filename = src.filename;
}

AUploadModule& AUploadModule::operator=(const AUploadModule& src)
{
	if (this != &src)
	{
		handler = src.handler;
		body_read = src.body_read;
		body_parsing_done = src.body_parsing_done;
		filename = src.filename;
	}
	return (*this);
}


////////// GETTERS ///////////

bool	AUploadModule::getUploadStatus() const
{
	return (body_read);
}


///////// METHODS ///////////

