#ifndef CGIRESPONSE2_H
# define CGIRESPONSE2_H

# include <vector>
# include <string>
# include "RequestHandler.h"
# include "AResponse.h"
# include "utils.tpp"

class CGIResponse2: public AResponse
{
private:
	/* data */
public:
	CGIResponse2(RequestHandler&);
	CGIResponse2(/* args */);
	~CGIResponse2();

	void		createResponse();
	void		setEnv();
	std::string	identifyPathInfo();

	char**	env;
};

#endif