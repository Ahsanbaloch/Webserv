#ifndef AREQUEST_H
# define AREQUEST_H

# include <string>
# include <map>
# include <vector>
# include "Response.h"

class RequestHandler;

class ARequest
{
private:
	static std::vector<std::string>	splitPath(std::string input, char delim);
	static int	calcMatches(std::vector<std::string>&, std::vector<std::string>&);
public:
	ARequest(/* args */);
	virtual ~ARequest();

	static ARequest *newRequest(RequestHandler&);

	static void	findServerBlock(RequestHandler&);
	static void	findLocationBlock(RequestHandler&);
	static bool	checkFileType(RequestHandler&);
	static int	checkFileExistence(RequestHandler&);

	virtual Response *createResponse(RequestHandler&) = 0;

	
};

#endif
