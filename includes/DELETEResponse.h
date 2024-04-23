#ifndef DELETERESPONSE_H
# define DELETERESPONSE_H

# include <iostream>
# include <sstream>
# include <cstdio>
# include "AResponse.h"
# include "RequestHandler.h"

class DELETEResponse: public AResponse
{
private:
	// helper methods
	void		deleteFile();
	void		deleteDir();

	// constructors
	DELETEResponse();
	DELETEResponse(const DELETEResponse&);
	DELETEResponse& operator=(const DELETEResponse&);

public:
	// constructors & destructors
	explicit DELETEResponse(RequestHandler&);
	~DELETEResponse();

	// main method
	void	createResponse();

};

#endif
