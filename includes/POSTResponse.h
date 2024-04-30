#ifndef POSTRESPONSE_H
# define POSTRESPONSE_H

# include <string>
# include "RequestHandler.h"
# include "AResponse.h"
# include "utils.tpp"

class POSTResponse: public AResponse
{
private:
	// helper methods
	void		respondFileUpload();
	std::string	createBody();

	// constructors
	POSTResponse();
	POSTResponse(const POSTResponse&);
	POSTResponse& operator=(const POSTResponse&);

public:
	// constructors & destructors
	explicit POSTResponse(RequestHandler&);
	~POSTResponse();

	// main method
	void		createResponse();
};

#endif
