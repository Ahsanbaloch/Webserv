#ifndef ERRORRESPONSE_H
# define ERRORRESPONSE_H

# include <string>
# include <fstream>
# include "AResponse.h"
# include "RequestHandler.h"
# include "utils.tpp"

class ERRORResponse: public AResponse
{
private:
	// helper methods
	std::string	getDefaultErrorMessage(std::string);
	std::string	createBody(std::string);
	void		appendAllowedMethods();
	void		removeTempFiles();

	// constructors
	ERRORResponse();
	ERRORResponse(const ERRORResponse&);
	ERRORResponse& operator=(const ERRORResponse&);

public:
	// constructors & destructors
	explicit ERRORResponse(RequestHandler&);
	~ERRORResponse();

	// main method
	void		createResponse();

};

#endif
