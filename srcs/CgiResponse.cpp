#include "CgiResponse.hpp"
#include <fstream>
#include <sys/fcntl.h>
#include <fcntl.h>
#include <errno.h>
#include <string>

bool is_fd_open(int fd) {
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

CgiResponse::CgiResponse(){}

CgiResponse::CgiResponse(RequestHandler& request_handler) : AResponse(request_handler), _envp(), _env(), _scriptPath(), _queryString(), _pathInfo(), _cgiOutputFd(), _cgiInputFd(){
}

CgiResponse::CgiResponse(const CgiResponse &other) : AResponse(other), _envp(other._envp), _env(other._env), _scriptPath(other._scriptPath), _queryString(other._queryString), _pathInfo(other._pathInfo), _cgiOutputStr(other._cgiOutputStr){
	*this = other;
}

CgiResponse::~CgiResponse() {
}

CgiResponse &CgiResponse::operator=(const CgiResponse &other) {
	if (this != &other) {
		_env = other._env;
		handler = other.handler;
		header_fields = other.header_fields;
	}
	return *this;
}

void CgiResponse::createResponse()
{
	_getScriptPath();
	_getQueryString();
	_getPathInfo();
	_setEnv();
	_execCgi();
}

void CgiResponse::_getScriptPath() {
	std::string path = handler.request_header.getPath();
	size_t pos = path.find('.');
	size_t pathInfo = path.find('/', pos);
	size_t queryString = path.find('?', pos);
	//std::cout << "path: " << path << std::endl;
	if (queryString < pathInfo)
		_scriptPath = path.substr(0, queryString);
	else 
		_scriptPath = path.substr(0, pathInfo);
	_scriptPath = handler.getLocationConfig().root + _scriptPath;
	//std::cout << "script path: " << _scriptPath << std::endl;
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
	_env.push_back("CONTENT_TYPE=" + handler.getHeaderInfo().getHeaderFields()["content-type"]);
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
	//_env.push_back("SERVER_PORT=" + handler.getSelectedServer().port);
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
	std::cout << "Started writing cgi input" << std::endl << std::endl;
	std::string temp_file_path = handler.getTempBodyFilepath();
	printf("temp_file_path: %s\n", temp_file_path.c_str());
	int temp_fd = open(temp_file_path.c_str(), O_RDONLY);
	if (temp_fd == -1)
		throw CustomException("CgiResponse: open() failed");
	printf("temp_fd: %d\n", temp_fd);
	char buf[500];
	int bytes_read = 0;
	bytes_read = read(temp_fd, buf, 499); {

		buf[bytes_read] = '\0';
		write(_cgiInputFd[1], buf, bytes_read);
		//write(_cgiInputFd[1], "\n\nTesting\n\n", 11);
		write(1, buf, bytes_read);
	}
}

void CgiResponse::_readCgiOutput() {
	char buf[10];
	int bytes_read;
	while ((bytes_read = read(_cgiOutputFd[0], buf, 9)) > 0) {
		buf[bytes_read] = '\0';		
		_cgiOutputStr += buf;
	}
	if (bytes_read == -1)
		perror("read");
}

void CgiResponse::_execCgi() {
    // Create pipes for input and output
    if (pipe(_cgiInputFd) < 0 || pipe(_cgiOutputFd) < 0) {
        throw CustomException("CgiResponse: Failed to create pipes");
    }
	printf("pipe done\n");
    // Fork a new process
    pid_t pid = fork();
	printf("fork done\n");
    if (pid < 0) {
        throw CustomException("CgiResponse: Failed to fork process");
    }

    if (pid == 0) { // Child process
        // Redirect standard input and output to the pipes
		printf("in child\n");
		dup2(_cgiInputFd[0], 0);
		dup2(_cgiOutputFd[1], 1);

        // Close the other ends of the pipes
        close(_cgiInputFd[1]);
        close(_cgiOutputFd[0]);
		close(_cgiInputFd[0]);
		close(_cgiOutputFd[1]);

        // Set up the environment for the CGI script
        _exportEnv();
		//printf("export done\n");
		// Change to the script directory
		std::string scriptPath = _scriptPath;
		std::string scriptName = scriptPath.substr(scriptPath.find_last_of('/') + 1);
		while (scriptPath.back() != '/')
			scriptPath.pop_back();
		scriptPath = "./" + scriptPath;
		
		//printf("scriptPath: %s\n", scriptPath.c_str());
        // Change to the script directory
        if (chdir(scriptPath.c_str()) < 0) {
            exit(1);
        }
		if (execve(scriptName.c_str(), NULL, _envp) < 0) {
			printf("execve failed\n");
			exit(1);
		}
    } else { // Parent process
        // Write the HTTP request body to the CGI script, if necessary
        printf("in parent\n");
		if (handler.request_header.getMethod() == "POST") {
            _writeCgiInput();
        }
        // Close the other ends of the pipes
        close(_cgiInputFd[0]);
        close(_cgiOutputFd[1]);
		close(_cgiInputFd[1]);

		std::cout << "Waiting for CGI script to finish execution" << std::endl;
        // Wait for the CGI script to finish execution
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            throw CustomException("CgiResponse: Failed to wait for CGI script");
        }

		std::cout << "WEXITSTATUS: " << WEXITSTATUS(status) << std::endl;
		std::cout << "WIFEXITED: " << WIFEXITED(status) << std::endl;
        // Check the exit status of the CGI script
        // if (WIFEXITED(status) && WEXITSTATUS(status) != 0){
		// 	close(_cgiOutputFd[0]);
        //     handler.setStatus(404);
        //     throw CustomException("CgiResponse: CGI script failed");
        // }

        // Read the output of the CGI script
		_readCgiOutput();
		close(_cgiOutputFd[0]);
        // Construct the HTTP response
			body = "\n\t\t<!DOCTYPE html>\n\t\t<html lang=\"en\">\n\t\t<head>\n\t\t\t<meta charset=\"UTF-8\">\n\t\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t\t\t<title>CGI Page</title>\n\t\t</head>\n\t";
			body.append(_cgiOutputStr);
			handler.setStatus(200);
			status_line = createStatusLine();
			header_fields = createHeaderFields(body);
    }
}

std::string	CgiResponse::identifyMIME()
{
	// also check against accept header? --> return 406 if the requirement cannot be satisfied
	// how to best identifyMIME?
	if (file_type == "html")
		return ("text/html");
	else if (file_type == "jpeg")
		return ("image/jpeg");
	else if (file_type == "png" || file_type == "ico")
		return ("image/png");
	else
		return ("text/html"); // what should be the default return?
}


std::string	CgiResponse::createHeaderFields(std::string body) // probably don't need parameter anymore
{
	std::string	header;

	if (!handler.getLocationConfig().redirect.empty())
		header.append("Location: " + handler.getLocationConfig().redirect + "\r\n");
	else
	{
		std::string mime_type = identifyMIME(); // should not be called if we have a url redirection
		if (mime_type.empty()) // only check when body should be sent?
		{
			handler.setStatus(415);
			throw CustomException("Unsupported Media Type");
		}
		else
		{
			std::ostringstream length_conversion;
			length_conversion << body.size();
			header.append("Content-Type: " + mime_type + "\r\n");
			header.append("Content-Length: "); // alternatively TE: chunked?
			header.append(length_conversion.str() + "\r\n");
			// header.append("Location: url"); // redirect client to a different url or new path 
			// what other headers to include?
			// send Repsonses in Chunks?
			// header.append("Transfer-Encoding: chunked");
			// header.append("Cache-Control: no-cache");
			// header.append("Set-Cookie: preference=darkmode; Domain=example.com");
			// header.append("Server: nginx/1.21.0");
			// header.append("Expires: Sat, 08 May 2023 12:00:00 GMT"); // If a client requests the same resource before the expiration date has passed, the server can return a cached copy of the resource.
			// header.append("Last-Modified: Tue, 04 May 2023 16:00:00 GMT"); // This header specifies the date and time when the content being sent in the response was last modified. This can be used by clients to determine if the resource has changed since it was last requested.
			// header.append("ETag: "abc123""); //This header provides a unique identifier for the content being sent in the response. This can be used by clients to determine if the resource has changed since it was last requested, without having to download the entire resource again.
			// header.append("Keep-Alive: timeout=5, max=100"); // used to enable persistent connections between the client and the server, allowing multiple requests and responses to be sent over a single TCP connection
			// Access-Control-Allow-Origin; X-Frame-Options; X-XSS-Protection; Referrer-Policy; X-Forwarded-For; X-Powered-By; 
			header.append("\r\n");
		}
	}
	return (header);
}
