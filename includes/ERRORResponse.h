#ifndef ERRORRESPONSE_H
# define ERRORRESPONSE_H

# include <string>
# include "AResponse.h"
# include "RequestHandler.h"

class ERRORResponse: public AResponse
{
private:
	/* data */
public:
	ERRORResponse(/* args */);
	explicit ERRORResponse(RequestHandler&);
	~ERRORResponse();

	void	createResponse();
	std::string getDefaultErrorMessage(std::string);
	std::string	createBody(std::string);
};

#endif
