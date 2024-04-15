#ifndef POSTRESPONSE_H
# define POSTRESPONSE_H

#include "RequestHandler.h"
#include "AResponse.h"

class POSTResponse: public AResponse
{
private:
	// helper methods
	void	respondFileUpload();

	// constructors
	POSTResponse();
	POSTResponse(const POSTResponse&);
	POSTResponse& operator=(const POSTResponse&);

public:
	// constructors & destructors
	explicit POSTResponse(RequestHandler&);
	~POSTResponse();

	// main method
	void	createResponse();
};

#endif
