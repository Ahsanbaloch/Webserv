#include "CGIHandler.h"
#include "RequestHandler.h"

CGIHandler::CGIHandler()
	: handler(*new RequestHandler())
{}

CGIHandler::CGIHandler(RequestHandler& src)
	: handler(src), _envp()
{
	if (pipe(cgi_out) < 0)
		throw CustomException("CGIHandler: Failed to create pipes");
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
		}
	return *this;
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

void	CGIHandler::createArgument()
{
	std::string file_path = "./www/cgi-bin/" + handler.getHeaderInfo().getFilename();
	if (handler.getHeaderInfo().getMethod() == "POST")
	{
		argv = new char*[3];
		argv[0] = strdup(file_path.c_str());
		std::cout << "temp body path" << handler.getTempBodyFilepath() << std::endl;
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

void CGIHandler::_execCgi() 
{
	createArgument();

	// // Fork a new process
	pid_t pid = fork();
	printf("fork done\n");
	if (pid < 0)
	{
		throw CustomException("CGIHandler: Failed to fork process");
    }

	if (pid == 0) { // Child process
		// Redirect standard output to the pipe
		dup2(cgi_out[1], STDOUT_FILENO);
		// check for error

		// close the end of the pipes
		close(cgi_out[0]);
		close(cgi_out[1]);

		// Change to the script directory?
		//printf("scriptPath: %s\n", scriptPath.c_str());
		// Change to the script directory
		// if (chdir(scriptPath.c_str()) < 0) {
		//     exit(1);
		// }

		if (execve(argv[0], argv, _envp) < 0)
		{
			perror("execve failure");
			exit(1);
		}
	}
	else 
	{
		close(cgi_out[1]);

		// put this somewhere else? // also introduce timer
		std::cout << "Waiting for CGI script to finish execution" << std::endl;
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
	}
}

