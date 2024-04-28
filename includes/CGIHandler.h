#ifndef CGIHANDLER_H
# define CGIHANDLER_H

# include <signal.h>
# include <string>
# include <fstream>
# include <sys/fcntl.h>
# include <fcntl.h>
# include <errno.h>
# include "AResponse.h"


class RequestHandler;

class CGIHandler
{
public:
    
    ~CGIHandler();
    CGIHandler(const CGIHandler &other);
	explicit CGIHandler(RequestHandler&);
    CGIHandler &operator=(const CGIHandler &other);

	void execute();

	int	cgi_out[2];

private:
	RequestHandler&	handler;
    CGIHandler();

    std::string createHeaderFields(std::string);
    std::string identifyMIME();
    void _execCgi();
   
    std::string identifyPathInfo();
    void setEnv();
	void	createArgument();
	void	createTempFile();

    char **_envp;
	char** argv;
  
};

#endif
