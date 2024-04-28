#include "CGIHandler.h"
#include "RequestHandler.h"

CGIHandler::CGIHandler()
	: handler(*new RequestHandler())
{}

CGIHandler::CGIHandler(RequestHandler& src)
	: handler(src), env(NULL)
{
	if (pipe(cgi_out) < 0)
	{
		handler.setStatus(500);
		throw CustomException("Internal Server Error: Failed to create pipe");
	}
}

CGIHandler::CGIHandler(const CGIHandler &other)
	: handler(other.handler)
{
	 *this = other;
}

CGIHandler::~CGIHandler()
{
	if (env != NULL)
	{
		for (size_t i = 0; env[i]; i++)
			delete env[i];
		delete env;
	}
	if (argv != NULL)
	{
		for (size_t i = 0; argv[i]; i++)
			delete argv[i];
		delete argv;
	}
}

CGIHandler &CGIHandler::operator=(const CGIHandler &other) 
{
	if (this != &other)
	{
		env = other.env;
		argv = other.argv;
		handler = other.handler;
	}
	return (*this);
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
	std::vector<std::string> temp_env;
	std::string path_info = identifyPathInfo();
	std::string doc_root = handler.getLocationConfig().root;
	std::string	cgi_location = "/cgi-bin/";

	temp_env.push_back("AUTH_TYPE=Basic");
	if (handler.getHeaderInfo().getBodyLength() == 0)
		temp_env.push_back("CONTENT_LENGTH=");
	else
		temp_env.push_back("CONTENT_LENGTH=" + toString(handler.getHeaderInfo().getBodyLength()));
	temp_env.push_back("CONTENT_TYPE=" + handler.getHeaderInfo().getHeaderFields()["content-type"]);
	temp_env.push_back("DOCUMENT_ROOT=" + doc_root);
	temp_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	temp_env.push_back("PATH_INFO=" + path_info);
	temp_env.push_back("PATH_TRANSLATED=" + doc_root + path_info);
	temp_env.push_back("QUERY_STRING=" + handler.getHeaderInfo().getQuery());
	// temp_env.push_back("REMOTE_ADDR=");
	// temp_env.push_back("REMOTE_HOST=");
	// temp_env.push_back("REMOTE_IDENT=");
	// temp_env.push_back("REMOTE_USER=");
	temp_env.push_back("REQUEST_METHOD=" + handler.getHeaderInfo().getMethod());
	// temp.push_back("REQUEST_URI=");
	temp_env.push_back("SCRIPT_NAME=" + cgi_location + handler.getHeaderInfo().getFilename());
	temp_env.push_back("SERVER_NAME=" + handler.getSelectedServer().Ip);
	temp_env.push_back("SERVER_PORT=" + toString(handler.getSelectedServer().port));
	temp_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	temp_env.push_back("SERVER_SOFTWARE=webserv");

	env = new char*[temp_env.size() + 1];
	for (size_t i = 0; i < temp_env.size(); i++)
	{
		env[i] = new char[temp_env[i].size() + 1];
		std::strcpy(env[i], temp_env[i].c_str());
	}
	env[temp_env.size()] = NULL;
	for (size_t i = 0; i < temp_env.size(); i++)
		std::cout << "env[" << i << "]: " << env[i] << std::endl;
}

// make program the first argument and path the second?
void	CGIHandler::createArguments()
{
	std::string file_path = "./www/cgi-bin/" + handler.getHeaderInfo().getFilename();
	if (handler.getHeaderInfo().getMethod() == "POST")
	{
		if (handler.getTempBodyFilepath().empty())
		{
			handler.setStatus(500);
			throw CustomException("Internal Server Error");
		}
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

void CGIHandler::_execCgi() 
{
	createArguments();
	cgi_pid = fork();
	if (cgi_pid < 0)
	{
		handler.setStatus(500);
		throw CustomException("Internal Server Error: Failed to fork process");
	}
	if (cgi_pid == 0)
	{
		if (dup2(cgi_out[1], STDOUT_FILENO) < 0)
		{
			handler.setStatus(500);
			throw CustomException("Internal Server Error: Failed to dup");
		}
		if (close(cgi_out[0]) < 0 || close(cgi_out[1] < 0))
		{
			handler.setStatus(500);
			throw CustomException("Internal Server Error: Failed to close pipe ends");
		}

		// Change to the script directory ??
		// if (chdir(scriptPath.c_str()) < 0) {
		//     exit(1);
		// }

		if (execve(argv[0], argv, env) < 0)
		{
			handler.setStatus(500);
			throw CustomException("Internal Server Error: Execve failed");
		}
	}
	else
	{
		close(cgi_out[1]);
		if (handler.getHeaderInfo().getMethod() == "POST")
			remove(handler.getTempBodyFilepath().c_str());
		int status;
		time_t start;
		std::time(&start);
		while (std::difftime(std::time(NULL), start) < 10)
		{
			waitpid(cgi_pid, &status, WNOHANG);
			if (WIFEXITED(status))
				break;
		}
		if (!WIFEXITED(status))
		{
			kill(cgi_pid, SIGKILL);
			close(cgi_out[0]);
			handler.setStatus(504);
			throw CustomException("Gateway Timeout");
		}
		if (WEXITSTATUS(status)) {
			close(cgi_out[0]);
			handler.setStatus(500);
			throw CustomException("Internal Server Error");
		}
	}
}

