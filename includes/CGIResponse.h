#ifndef CGIRESPONSE_H
# define CGIRESPONSE_H

# include <vector>
# include <string>
# include "RequestHandler.h"
# include "AResponse.h"
# include "utils.tpp"

class CGIResponse: public AResponse
{
private:
	/* data */
public:
	CGIResponse(RequestHandler&);
	CGIResponse(/* args */);
	~CGIResponse();

	void	createResponse();
};

#endif
