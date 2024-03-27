#ifndef ERRORREQUEST_H
# define ERRORREQUEST_H

# include <string>
# include "ARequest.h"
# include "RequestHandler.h"

class ERRORRequest: public ARequest
{
private:
	/* data */
public:
	ERRORRequest(/* args */);
	explicit ERRORRequest(RequestHandler&);
	~ERRORRequest();

	void	createResponse();
};




#endif
