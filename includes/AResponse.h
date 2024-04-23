#ifndef ARESPONSE_H
# define ARESPONSE_H

# include <string>
# include <map>
# include <vector>
# include "utils.tpp"

class RequestHandler;

class AResponse
{
protected:
	RequestHandler&	handler;

	// vars
	std::string		file_type;
	std::string		full_file_path;
	
	std::string		body;
	std::string		status_line;
	std::string		header_fields;

	// flags
	bool			response_complete;
	bool			chunked_body;

	// methods
	std::string		createStatusLine();

	// constructors
	AResponse();
	AResponse(const AResponse&);
	AResponse& operator=(const AResponse&);

public:
	// constructors & destructors
	explicit AResponse(RequestHandler&);
	virtual ~AResponse();

	// getters
	std::string		getResponseBody() const;
	std::string		getResponseStatusLine() const;
	std::string		getRespondsHeaderFields() const;
	std::string		getFullFilePath() const; // needed?
	bool			getResponseCompleteStatus() const;
	bool			getChunkedBodyStatus() const;

	// methods
	virtual void	createResponse() = 0;

};

#endif
