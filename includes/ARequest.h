#ifndef AREQUEST_H
# define AREQUEST_H

# include <string>
# include <map>

class ARequest
{
private:
	/* data */
public:
	ARequest(/* args */);
	virtual ~ARequest();

	std::string							query;
	std::string							path;
	std::map<std::string, std::string>	headers;

	// somewhere create a virtual function and set to 0

	static ARequest *newRequest();

	
};

#endif
