#ifndef AREQUEST_H
# define AREQUEST_H

# include <string>
# include <map>
# include <vector>
# include "Response.h"

class RequestHandler;

class ARequest
{
protected:
	RequestHandler&	handler;

	// vars
	std::string			file_type;
	std::string			redirected_path;

	// methods
	bool			checkFileType();
	int				checkFileExistence();

	// constructors
	ARequest();
	ARequest(const ARequest&);
	ARequest& operator=(const ARequest&);

public:
	// constructors & destructors
	explicit ARequest(RequestHandler&);
	virtual ~ARequest();

	// getters
	std::string			getRedirectedPath() const;

	// methods
	virtual Response	*createResponse() = 0;

};

#endif
