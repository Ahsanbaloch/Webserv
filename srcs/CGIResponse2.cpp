
#include "CGIResponse2.h"

CGIResponse2::CGIResponse2(RequestHandler& src)
	: AResponse(src)
{

}

CGIResponse2::CGIResponse2(/* args */)
	: AResponse()
{
}

CGIResponse2::~CGIResponse2()
{
}


void	CGIResponse2::createResponse()
{
	setEnv();
}

std::string	CGIResponse2::identifyPathInfo()
{
	std::size_t identify_path = handler.getHeaderInfo().getPath().find(handler.getHeaderInfo().getFilename());

	std::string path_info = handler.getHeaderInfo().getPath().substr(identify_path + handler.getHeaderInfo().getFilename().length());
	if (path_info.empty())
		path_info = "/";
	return (path_info);
}


// what if internal redirect --> then path needs to be identified differently
void	CGIResponse2::setEnv()
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

	env = new char*[temp.size() + 1];
	for (size_t i = 0; i < temp.size(); i++)
	{
		env[i] = new char[temp[i].size() + 1];
		std::strcpy(env[i], temp[i].c_str());
	}
	env[temp.size()] = NULL;

	throw CustomException("CGI stop");

	// AUTH_TYPE
	// CONTENT_LENGTH
	// CONTENT_TYPE
	// DOCUMENT_ROOT
	// GATEWAY_INTERFACE
	// PATH_INFO
	// PATH_TRANSLATED
	// QUERY_STRING
	// REMOTE_ADDR
	// REMOTE_HOST
	// REMOTE_IDENT
	// REMOTE_USER
	// REQUEST_METHOD
	// REQUEST_URI
	// SCRIPT_NAME
	// SERVER_NAME
	// SERVER_PORT
	// SERVER_PROTOCOL
	// SERVER_SOFTWARE

}
