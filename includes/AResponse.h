#ifndef ARESPONSE_H
# define ARESPONSE_H

# include <string>
# include <map>
# include <vector>
# include <fstream>
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

	std::ifstream	body_file;
	std::streampos	body_size;
	std::streampos	file_position;
	std::streampos	file_pos_offset;

	// flags
	bool			chunked_body;

	// class methods
	std::string		createStatusLine();
	std::string		readHTMLTemplateFile(const std::string&);
	void			openBodyFile(std::string);
	
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
	std::ifstream&	getBodyFile();
	bool			getChunkedBodyStatus() const;
	std::string		createBodyChunk();
	std::streampos	getBodySize() const;
	std::streampos	getFilePosition() const;
	std::streampos	getFilePosOffset() const;


	// main methods
	virtual void	createResponse() = 0;
	void			incrementFilePosition(std::streampos);

};

#endif
