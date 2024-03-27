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

	std::string	body;
	std::string status_line;
	std::string header_fields;

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
	std::string			getResponseBody() const;
	std::string			getResponseStatusLine() const;
	std::string			getRespondsHeaderFields() const;

	// methods
	virtual void		createResponse() = 0;
	void				errorResponse();

};

#endif
