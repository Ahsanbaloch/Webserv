
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
	filepath_outfile = src.filepath_outfile;
}

AUploadModule& AUploadModule::operator=(const AUploadModule& src)
{
	if (this != &src)
	{
		handler = src.handler;
		body_read = src.body_read;
		body_parsing_done = src.body_parsing_done;
		filepath_outfile = src.filepath_outfile;
	}
	return (*this);
}


////////// GETTERS ///////////

bool	AUploadModule::getUploadStatus() const
{
	return (body_read);
}


///////// METHODS ///////////

std::string	AUploadModule::getUploadDir()
{
	std::string upload_dir;

	if (handler.getLocationConfig().root[handler.getLocationConfig().root.length() - 1] == '/')
		upload_dir.append(handler.getLocationConfig().root);
	else
		upload_dir.append(handler.getLocationConfig().root + "/");
	if (handler.getLocationConfig().uploadDir[0] == '/')
		upload_dir.append(handler.getLocationConfig().uploadDir, 1);
	else
		upload_dir.append(handler.getLocationConfig().uploadDir);
	if (upload_dir[upload_dir.length() - 1] != '/')
		upload_dir.append("/");

	return (upload_dir);
}
