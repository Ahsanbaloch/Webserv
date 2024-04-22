
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

std::string	AUploadModule::getRelativeFilePath()
{
	std::string relative_path;
	if (handler.content_type == handler.urlencoded)
	{
		std::string temp_root = "www";
		relative_path = filepath_outfile.substr(temp_root.length());
	}
	else
		relative_path = filepath_outfile.substr(handler.getLocationConfig().root.length());
	return (relative_path);
}


///////// METHODS ///////////

std::string	AUploadModule::getUploadDir()
{
	std::string upload_dir = handler.getLocationConfig().root + handler.getLocationConfig().path + handler.getLocationConfig().uploadDir;

	// this would be ideally done by configParser for improved efficiency
	for (std::string::iterator it = upload_dir.begin(); it != upload_dir.end(); it++)
	{
		if (*it == '/')
		{
			it++;
			while (*it == '/')
				it = upload_dir.erase(it);
		}
	}
	if (upload_dir[upload_dir.length() - 1] != '/')
		upload_dir.append("/");
	return (upload_dir);
}
