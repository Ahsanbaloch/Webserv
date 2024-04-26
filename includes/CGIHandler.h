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
	std::string	getCGIOutput();

	std::string _cgiOutputStr;


private:
	RequestHandler&	handler;
    CGIHandler();

    std::string createHeaderFields(std::string);
    std::string identifyMIME();
    void _writeCgiInput();
    void _readCgiOutput();
    void _execCgi();
   
    std::string identifyPathInfo();
    void setEnv();
	void	createArgument();
	void	createTempFile();

    char **_envp;
	char** argv;
	std::string	temp_file_path;


    int _cgiOutputFd[2];
    int _cgiInputFd[2];
	int fd_out;
  
};

#endif
