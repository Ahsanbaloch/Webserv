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

	// methods
	virtual Response	*createResponse() = 0;

};

#endif
