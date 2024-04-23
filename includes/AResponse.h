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
	std::string			full_file_path;
	std::string			body;
	std::string 		status_line;
	std::string 		header_fields;

	// flags
	bool				response_complete;

	// methods
	std::string			buildPathFromLocationIndex();
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
	std::string			getResponseBody() const;
	std::string			getResponseStatusLine() const;
	std::string			getRespondsHeaderFields() const;
	std::string			getFullFilePath() const; // needed?

	// getters
	bool				getResponseCompleteStatus() const;

	// methods
	virtual void		createResponse() = 0;

	int								num_response_chunks2;

};

#endif
