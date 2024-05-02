#ifndef CGIHANDLER_H
# define CGIHANDLER_H

# include <signal.h>
# include <string.h>
# include <ctime>
# include <string>
# include <fstream>
# include <sys/fcntl.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <errno.h>
# include "AResponse.h"


class RequestHandler;

class CGIHandler
{
private:
	RequestHandler&	handler;

	// vars
	pid_t		cgi_pid;
	char**		env;
	char**		argv;

	// helper methods
	std::string	identifyPathInfo();
	void		execCGI();
	void 		setEnv();
	void		createArguments();
	void		createTempFile();

	// constructors
	CGIHandler();
	CGIHandler(const CGIHandler &other);
	CGIHandler &operator=(const CGIHandler &other);

public:
	// constructors & destructors
	~CGIHandler();
	explicit CGIHandler(RequestHandler&);

	// getters
	pid_t		getCGIPid() const;

	// pipe
	int	cgi_out[2];

	// main methods
	void execute();
	
};

#endif
