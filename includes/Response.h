#ifndef RESPONSE_H
# define RESPONSE_H

# include <string>

class RequestHandler;

class Response
{
private:
	/* data */
public:
	Response(/* args */);
	~Response();

	std::string	body;
	std::string status_line;
	std::string header_fields;

	void	errorResponse(RequestHandler&);

};


#endif
