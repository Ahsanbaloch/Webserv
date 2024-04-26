#include "CGIHandler.h"
#include "RequestHandler.h"

bool is_fd_open(int fd) {
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

CGIHandler::CGIHandler()
	: handler(*new RequestHandler())
{}

CGIHandler::CGIHandler(RequestHandler& src)
	: handler(src), _envp(), _cgiOutputFd(), _cgiInputFd()
{
}

CGIHandler::CGIHandler(const CGIHandler &other)
	: handler(other.handler)
{
	 *this = other;
}

CGIHandler::~CGIHandler() {
}

CGIHandler &CGIHandler::operator=(const CGIHandler &other) 
{
	if (this != &other) {
		_envp = other._envp;
		argv = other.argv;
		handler = other.handler;
		_cgiOutputStr = other._cgiOutputStr;
	}
	return *this;
}

std::string	CGIHandler::getCGIOutput()
{
	return (temp_file_path);
}

void CGIHandler::execute()
{
	setEnv();
	_execCgi();
}

std::string	CGIHandler::identifyPathInfo()
{
	std::size_t identify_path = handler.getHeaderInfo().getPath().find(handler.getHeaderInfo().getFilename());

	std::string path_info = handler.getHeaderInfo().getPath().substr(identify_path + handler.getHeaderInfo().getFilename().length());
	if (path_info.empty())
		path_info = "/";
	return (path_info);
}


// what if internal redirect --> then path needs to be identified differently
void	CGIHandler::setEnv()
{
	std::vector<std::string> temp;
	std::string path_info = identifyPathInfo();
	std::string doc_root = handler.getLocationConfig().root;
	std::string	cgi_location = "/cgi-bin/";

	temp.push_back("AUTH_TYPE=Basic");
	if (handler.getHeaderInfo().getBodyLength() == 0)
		temp.push_back("CONTENT_LENGTH=");
	else
		temp.push_back("CONTENT_LENGTH=" + toString(handler.getHeaderInfo().getBodyLength()));
	temp.push_back("CONTENT_TYPE=" + handler.getHeaderInfo().getHeaderFields()["content-type"]);
	temp.push_back("DOCUMENT_ROOT=" + doc_root);
	temp.push_back("GATEWAY_INTERFACE=CGI/1.1");
	temp.push_back("PATH_INFO=" + path_info);
	temp.push_back("PATH_TRANSLATED=" + doc_root + path_info);
	temp.push_back("QUERY_STRING=" + handler.getHeaderInfo().getQuery());
	// temp.push_back("REMOTE_ADDR=");
	// temp.push_back("REMOTE_HOST=");
	// temp.push_back("REMOTE_IDENT=");
	// temp.push_back("REMOTE_USER=");
	temp.push_back("REQUEST_METHOD=" + handler.getHeaderInfo().getMethod());
	// temp.push_back("REQUEST_URI=");
	temp.push_back("SCRIPT_NAME=" + cgi_location + handler.getHeaderInfo().getFilename());
	temp.push_back("SERVER_NAME=" + handler.getSelectedServer().Ip);
	temp.push_back("SERVER_PORT=" + toString(handler.getSelectedServer().port));
	temp.push_back("SERVER_PROTOCOL=HTTP/1.1");
	temp.push_back("SERVER_SOFTWARE=webserv");

	_envp = new char*[temp.size() + 1];
	for (size_t i = 0; i < temp.size(); i++)
	{
		_envp[i] = new char[temp[i].size() + 1];
		std::strcpy(_envp[i], temp[i].c_str());
	}
	_envp[temp.size()] = NULL;
	for (size_t i = 0; i < temp.size(); i++)
		std::cout << "envp[" << i << "]: " << _envp[i] << std::endl;
}

void CGIHandler::_readCgiOutput() {
	char buf[10];
	int bytes_read;
	while ((bytes_read = read(_cgiOutputFd[0], buf, 9)) > 0) {
		buf[bytes_read] = '\0';		
		_cgiOutputStr += buf;
	}
	if (bytes_read == -1)
		perror("read");
}

void	CGIHandler::createArgument()
{
	std::string file_path = "./www/cgi-bin/" + handler.getHeaderInfo().getFilename();
	if (handler.getHeaderInfo().getMethod() == "POST")
	{
		argv = new char*[3];
		argv[0] = strdup(file_path.c_str());
		argv[1] = strdup(handler.getTempBodyFilepath().c_str());
		argv[2] = NULL;
	}
	else
	{
		argv = new char*[2];
		argv[0] = strdup(file_path.c_str());
		argv[1] = NULL;
	}
}

void	CGIHandler::createTempFile()
{
	temp_file_path = "www/temp/cgi.bin";
	fd_out = open(temp_file_path.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd_out == -1)
		throw CustomException("CGIHandler: Failed to create temp file");
}

void CGIHandler::_execCgi() {
    // Create pipes for input and output

	createArgument();
	createTempFile();
    // if (pipe(_cgiInputFd) < 0 || pipe(_cgiOutputFd) < 0) {
    //     throw CustomException("CGIHandler: Failed to create pipes");
    // }
	// printf("pipe done\n");
    // // Fork a new process
    pid_t pid = fork();
	printf("fork done\n");
    if (pid < 0) {
        throw CustomException("CGIHandler: Failed to fork process");
    }


    if (pid == 0) { // Child process
        // Redirect standard input and output to the pipes
		printf("in child\n");
		// dup2(_cgiInputFd[0], 0);
		// dup2(_cgiOutputFd[1], 1);
		dup2(fd_out, STDOUT_FILENO);
		// check for error


        // Close the other ends of the pipes
        // close(_cgiInputFd[1]);
        // close(_cgiOutputFd[0]);
		// close(_cgiInputFd[0]);
		// close(_cgiOutputFd[1]);

        // Set up the environment for the CGI script
        //_exportEnv();
		//printf("export done\n");
		// Change to the script directory
		//std::string scriptPath = _scriptPath;
		std::string scriptPath = "www/cgi-bin/" + handler.getHeaderInfo().getFilename();
		std::string scriptName = scriptPath.substr(scriptPath.find_last_of('/') + 1);
		// while (scriptPath.back() != '/')
		// 	scriptPath.pop_back();
		scriptPath = "./" + scriptPath;
		
		//printf("scriptPath: %s\n", scriptPath.c_str());
        // Change to the script directory
        // if (chdir(scriptPath.c_str()) < 0) {
        //     exit(1);
        // }

		// std::cout << "scriptPath: " << scriptPath << std::endl;
		// std::cout << "scriptName: " << scriptName << std::endl;
		if (execve(scriptPath.c_str(), argv, _envp) < 0) {
			printf("execve failed\n");
			exit(1);
		}
    } else { // Parent process
        // Write the HTTP request body to the CGI script, if necessary
        printf("in parent\n");
		// if (handler.getHeaderInfo().getMethod() == "POST") {
        //     _writeCgiInput();
        // }
        // Close the other ends of the pipes
        // close(_cgiInputFd[0]);
        // close(_cgiOutputFd[1]);
		// close(_cgiInputFd[1]);

		std::cout << "Waiting for CGI script to finish execution" << std::endl;
        // Wait for the CGI script to finish execution
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            throw CustomException("CGIHandler: Failed to wait for CGI script");
        }
		if (handler.getHeaderInfo().getMethod() == "POST")
			remove(handler.getTempBodyFilepath().c_str());
		std::cout << "WEXITSTATUS: " << WEXITSTATUS(status) << std::endl;
		std::cout << "WIFEXITED: " << WIFEXITED(status) << std::endl;
        // Check the exit status of the CGI script
        // if (WIFEXITED(status) && WEXITSTATUS(status) != 0){
		// 	close(_cgiOutputFd[0]);
        //     handler.setStatus(404);
        //     throw CustomException("CGIHandler: CGI script failed");
        // }

        // Read the output of the CGI script
		// _readCgiOutput();
		// close(_cgiOutputFd[0]);
		close(fd_out);
        // Construct the HTTP response
		
    }
}

