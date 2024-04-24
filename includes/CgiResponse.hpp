#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include "AResponse.h"
#include "RequestHandler.h"
#include <signal.h>

class CgiResponse : public AResponse {
public:
    
    ~CgiResponse();
    CgiResponse(const CgiResponse &other);
	CgiResponse(RequestHandler&);
    CgiResponse &operator=(const CgiResponse &other);

    RequestHeader *header();

private:
    CgiResponse();

    std::string createHeaderFields(std::string);
    std::string identifyMIME();
    void _getScriptPath();
    void _getQueryString();
    void _getPathInfo();
    void _writeCgiInput();
    void _readCgiOutput();
    void _setEnv();
    void _exportEnv();
    void _execCgi();
    void createResponse();

    char **_envp;
    std::vector<std::string> _env;
    std::string _scriptPath;   
    std::string _queryString;
    std::string _pathInfo;
    std::string _boundary;

    std::string _cgiOutputStr;

    int _cgiOutputFd[2];
    int _cgiInputFd[2];
};

#endif
