#ifndef ARESPONSE_H
# define ARESPONSE_H

# include <string>
# include <map>
# include <vector>

class RequestHandler;

class AResponse
{
protected:
	RequestHandler&	handler;

	// vars
	std::string			file_type;
	std::string			redirected_path;

	std::string			body;
	std::string 		status_line;
	std::string 		header_fields;

	// flags
	bool				internal_redirect;

	// methods
	bool				checkFileType();
	void				identifyRedirectedPath();
	int					checkFileExistence(std::string);
	std::string			createStatusLine();

	// constructors
	AResponse();
	AResponse(const AResponse&);
	AResponse& operator=(const AResponse&);

public:
	// constructors & destructors
	explicit AResponse(RequestHandler&);
	virtual ~AResponse();

	// getters
	std::string			getRedirectedPath() const;
	std::string			getResponseBody() const;
	std::string			getResponseStatusLine() const;
	std::string			getRespondsHeaderFields() const;
	bool				getInternalRedirectStatus() const;

	// methods
	virtual void		createResponse() = 0;

};

#endif
