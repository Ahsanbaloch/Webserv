#ifndef ERRORRESPONSE_H
# define ERRORRESPONSE_H

# include <string>
# include "AResponse.h"
# include "RequestHandler.h"

class ERRORResponse: public AResponse
{
private:
	// helper methods
	std::string getDefaultErrorMessage(std::string);
	std::string	createBody(std::string);

	// constructors
	ERRORResponse();
	ERRORResponse(const ERRORResponse&);
	ERRORResponse& operator=(const ERRORResponse&);

public:
	// constructors & destructors
	explicit ERRORResponse(RequestHandler&);
	~ERRORResponse();

	// main method
	void	createResponse();

};

#endif
