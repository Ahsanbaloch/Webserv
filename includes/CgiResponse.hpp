#ifndef CGI_RESPONSE_HPP
# define CGI_RESPONSE_HPP

# include <signal.h>
# include <string>
# include <fstream>
# include <sys/fcntl.h>
# include <fcntl.h>
# include <errno.h>
# include "AResponse.h"


class RequestHandler;

class CgiResponse
{
public:
    
    ~CgiResponse();
    CgiResponse(const CgiResponse &other);
	explicit CgiResponse(RequestHandler&);
    CgiResponse &operator=(const CgiResponse &other);

	void createResponse();

	std::string _cgiOutputStr;


private:
	RequestHandler&	handler;
    CgiResponse();

    std::string createHeaderFields(std::string);
    std::string identifyMIME();
    void _writeCgiInput();
    void _readCgiOutput();
    void _execCgi();
   
    std::string identifyPathInfo();
    void setEnv();
	void	createArgument();

    char **_envp;
	char** argv;


    int _cgiOutputFd[2];
    int _cgiInputFd[2];
  
};

#endif
