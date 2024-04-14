#ifndef POSTRESPONSE_H
# define POSTRESPONSE_H

#include "RequestHandler.h"
#include "AResponse.h"

class POSTResponse: public AResponse
{
private:
	void	respondFileUpload();
	void	respondFormSubmission();
public:
	// constructors & destructors
	explicit POSTResponse(RequestHandler&);
	~POSTResponse();

	void	createResponse();
};

#endif
