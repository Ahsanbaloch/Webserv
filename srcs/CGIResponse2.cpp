
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
	std::map<std::string, std::string> temp;
	std::string path_info = identifyPathInfo();
	std::string doc_root = handler.getLocationConfig().root;
	std::string	cgi_location = "/cgi-bin/";

	temp.insert(std::pair<std::string, std::string>("AUTH_TYPE=","Basic"));
	if (handler.getHeaderInfo().getBodyLength() == 0)
		temp.insert(std::pair<std::string, std::string>("CONTENT_LENGTH=",""));
	else
		temp.insert(std::pair<std::string, std::string>("CONTENT_LENGTH=", toString(handler.getHeaderInfo().getBodyLength())));
	temp.insert(std::pair<std::string, std::string>("CONTENT_TYPE=", handler.getHeaderInfo().getHeaderFields()["content-type"]));
	temp.insert(std::pair<std::string, std::string>("DOCUMENT_ROOT=", doc_root));
	temp.insert(std::pair<std::string, std::string>("GATEWAY_INTERFACE=","CGI/1.1"));
	temp.insert(std::pair<std::string, std::string>("PATH_INFO=", path_info));
	temp.insert(std::pair<std::string, std::string>("PATH_TRANSLATED=", doc_root + path_info));
	temp.insert(std::pair<std::string, std::string>("QUERY_STRING=", handler.getHeaderInfo().getQuery()));
	// temp.insert(std::pair<std::string, std::string>("REMOTE_ADDR=",""));
	// temp.insert(std::pair<std::string, std::string>("REMOTE_HOST=",""));
	// temp.insert(std::pair<std::string, std::string>("REMOTE_IDENT=",""));
	// temp.insert(std::pair<std::string, std::string>("REMOTE_USER=",""));
	temp.insert(std::pair<std::string, std::string>("REQUEST_METHOD=", handler.getHeaderInfo().getMethod()));
	temp.insert(std::pair<std::string, std::string>("REQUEST_URI=", handler.getHeaderInfo().getPath()));
	temp.insert(std::pair<std::string, std::string>("SCRIPT_NAME=", cgi_location + handler.getHeaderInfo().getFilename()));
	temp.insert(std::pair<std::string, std::string>("SERVER_NAME=", handler.getSelectedServer().Ip));
	temp.insert(std::pair<std::string, std::string>("SERVER_PORT=", toString(handler.getSelectedServer().port)));
	temp.insert(std::pair<std::string, std::string>("SERVER_PROTOCOL=", "HTTP/1.1"));
	temp.insert(std::pair<std::string, std::string>("SERVER_SOFTWARE=", "webserv"));

	for (std::map<std::string, std::string>::iterator it = temp.begin(); it != temp.end(); it++)
	{
		std::cout << "key: " << it->first << " value: " << it->second << std::endl;
	}

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
