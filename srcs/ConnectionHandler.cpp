
#include "ConnectionHandler.h"

ConnectionHandler::ConnectionHandler()
{
	connection_fd = 0;
	response_ready = 0;
	handler = NULL;
}

ConnectionHandler::ConnectionHandler(const ConnectionHandler& src)
{
	handler = src.handler;
	server_config = src.server_config;
	connection_fd = src.connection_fd;
	response_ready = src.response_ready;
}

ConnectionHandler&	ConnectionHandler::operator=(const ConnectionHandler& src)
{
	if (this != &src)
	{
		handler = src.handler;
		server_config = src.server_config;
		connection_fd = src.connection_fd;
		response_ready = src.response_ready;
	}
	return (*this);
}

ConnectionHandler::ConnectionHandler(int fd, std::vector<t_server_config> server_config)
{
	connection_fd = fd;
	response_ready = 0;
	this->server_config = server_config;
	handler = NULL;
}

ConnectionHandler::~ConnectionHandler()
{
}

RequestHandler*	ConnectionHandler::getRequestHandler()
{
	return (handler);
}

void	ConnectionHandler::initRequestHandler()
{
	handler = new RequestHandler(connection_fd, server_config);
}

int	ConnectionHandler::getResponseStatus()
{
	return (response_ready);
}

void	ConnectionHandler::setResponseStatus(int response_status)
{
	response_ready = response_status;
}

void	ConnectionHandler::removeRequestHandler()
{
	delete handler;
	handler = NULL;
}
