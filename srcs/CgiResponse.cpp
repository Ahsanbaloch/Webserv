#include "CgiResponse.hpp"

CgiResponse::CgiResponse(){}

CgiResponse::CgiResponse(RequestHandler &request_handler) : AResponse(request_handler), _envp(), _env(), _scriptPath(), _queryString(), _pathInfo(), _cgiInputFd(), _cgiOutputFd() {
}

CgiResponse::CgiResponse(const CgiResponse &other) : AResponse(other), _envp(other._envp), _env(other._env), _scriptPath(other._scriptPath), _queryString(other._queryString), _pathInfo(other._pathInfo) {
	*this = other;
}

CgiResponse::~CgiResponse() {
}

CgiResponse &CgiResponse::operator=(const CgiResponse &other) {
	if (this != &other) {
		_env = other._env;
		handler = other.handler;
		header_fields = other.header_fields;
		redirected_path = other.redirected_path;
	}
	return *this;
}

RequestHeader *CgiResponse::header() {
	RequestHeader *header = new RequestHeader(handler);
	std::cout << "CgiResponse::header()" << std::endl;

	_getScriptPath();
	_getQueryString();
	_getPathInfo();
	_setEnv();
	_execCgi(header);
	return header;
}

void CgiResponse::_getScriptPath() {
	std::string path = handler.request_header.getPath();
	size_t pos = path.find('.');
	size_t pathInfo = path.find('/', pos);
	size_t queryString = path.find('?', pos);
	if (queryString < pathInfo)
		_scriptPath = path.substr(0, queryString);
	else 
		_scriptPath = path.substr(0, pathInfo);
	if (access(_scriptPath.c_str(), F_OK) == -1)
		throw CustomException("CgiResponse: script path does not exist");
	if (access(_scriptPath.c_str(), X_OK) == -1)
		throw CustomException("CgiResponse: script path is not executable");
}

void CgiResponse::_getQueryString() {
	std::string path = handler.request_header.getPath();
	size_t pos = path.find('?');
	if (pos != std::string::npos)
		_queryString = path.substr(pos + 1);
}

void CgiResponse::_getPathInfo() {
	std::string path = handler.request_header.getPath();
	size_t pos = path.find('.');
	size_t pathInfo = path.find('/', pos);
	size_t queryString = path.find('?', pos);
	if (pathInfo != std::string::npos && pathInfo != std::string::npos)
		_pathInfo = path.substr(pathInfo, queryString - pathInfo);
	else if (pathInfo != std::string::npos)
		_pathInfo = path.substr(pathInfo);
	else
		_pathInfo = "";
}

void CgiResponse::_setEnv() {
	_env.push_back("AUTH_TYPE=" + handler.getHeaderInfo().getHeaderFields()["Authorization"]);
	_env.push_back("CONTENT_LENGTH=" + std::to_string(handler.request_header.getBodyLength()));
	_env.push_back("CONTENT_TYPE=" + handler.getHeaderInfo().getHeaderFields()["Content-Type"]);
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_env.push_back("PATH_INFO=" + _pathInfo);
	_env.push_back("PATH_TRANSLATED=");
	_env.push_back("QUERY_STRING=" + _queryString);
	_env.push_back("REMOTE_ADDR=");
	_env.push_back("REMOTE_IDENT=");
	_env.push_back("REMOTE_USER=");
	_env.push_back("REQUEST_METHOD=" + handler.request_header.getMethod());
	_env.push_back("REQUEST_URI=" + handler.request_header.getPath());
	_env.push_back("SCRIPT_NAME=" + _scriptPath);
	_env.push_back("SERVER_NAME=" + handler.getSelectedServer().serverName);
	_env.push_back("SERVER_PORT=" + handler.getSelectedServer().port);
	_env.push_back("SERVER_PROTOCOL=" + handler.getHeaderInfo().getHttpVersion());
	_env.push_back("SERVER_SOFTWARE=webserv");
}

void CgiResponse::_exportEnv() {
	_envp = new char*[_env.size() + 1];
	for (size_t i = 0; i < _env.size(); i++) {
		_envp[i] = new char[_env[i].size() + 1];
		std::strcpy(_envp[i], _env[i].c_str());
	}
	_envp[_env.size()] = NULL;
}

void CgiResponse::_writeCgiInput() {
	write(_cgiInputFd[1], &getResponseBody(), getResponseBody().size());
}

void CgiResponse::_readCgiOutput() {
	char buf[1024];
	int bytes_read;
	while ((bytes_read = read(_cgiOutputFd[0], buf, 1024)) > 0) {
		buf[bytes_read] = '\0';
		_cgiOutputStr += buf;
	}
}

void CgiResponse::_execCgi(RequestHeader *header) {
	pid_t pid;
	int status;

	if (pipe(_cgiInputFd) == -1 || pipe(_cgiOutputFd) == -1)
		throw CustomException("CgiResponse: pipe() failed");
	if ((pid = fork()) == -1)
		throw CustomException("CgiResponse: fork() failed");
	if (pid == 0)
	{
		dup2(_cgiInputFd[0], 0);
		dup2(_cgiOutputFd[1], 1);
		close(_cgiInputFd[1]);
		close(_cgiOutputFd[0]);
		close(_cgiInputFd[0]);
		close(_cgiOutputFd[1]);
		_exportEnv();
		std::string scriptPath = _scriptPath;
		std::string scriptName = scriptPath.substr(scriptPath.find_last_of('/') + 1);
		while (scriptPath.back() != '/')
			scriptPath.pop_back();
		scriptPath = "./" + scriptPath;
		if (chdir(scriptPath.c_str()) == 1)
			exit(1);
		if (execve(scriptName.c_str(), NULL, _envp) == -1)
			exit(1);
	}
	else
	{
		if (handler.request_header.getMethod() == "POST")
			_writeCgiInput();
		close(_cgiInputFd[1]);
		close(_cgiInputFd[0]);
		close(_cgiOutputFd[1]);
		time_t start;
		std:time(&start);
		while(std::difftime(std::time(0), start) < 5)
		{
			waitpid(pid, &status, WNOHANG);
			if (WIFEXITED(status))
				break;
		}
		if (!WIFEXITED(status))
		{
			kill(pid, SIGKILL);
			close(_cgiOutputFd[0]);
			throw CustomException("CgiResponse: CGI script timed out");
		}
		if (WEXITSTATUS(status) != 0)
		{
			close(_cgiOutputFd[0]);
			throw CustomException("CgiResponse: CGI script failed");
		}
		_readCgiOutput();
		close(_cgiOutputFd[0]);
		size_t pos = _cgiOutputStr.find("\r\n\r\n");
		if (pos != std::string::npos)
		{
			body = _cgiOutputStr.substr(0, pos);
			
			header_fields = GETResponse::createHeaderFields(_cgiOutputStr.substr(pos + 4));
			//handler.
			//handler.response->setHeaderFields(_cgiOutputStr.substr(pos + 4));
		}
		else
			body = _cgiOutputStr;
			//handler.response->setBody(_cgiOutputStr);
	}
}