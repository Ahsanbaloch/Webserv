
#include "ConnectionHandler.h"

ConnectionHandler::ConnectionHandler()
{
	connection_fd = -1;
	kernel_q_fd = -1;
	response_ready = 0;
	handler = NULL;
}

ConnectionHandler::ConnectionHandler(const ConnectionHandler& src)
{
	handler = src.handler;
	server_config = src.server_config;
	connection_fd = src.connection_fd;
	kernel_q_fd = src.kernel_q_fd;
	response_ready = src.response_ready;
}

ConnectionHandler&	ConnectionHandler::operator=(const ConnectionHandler& src)
{
	if (this != &src)
	{
		handler = src.handler;
		server_config = src.server_config;
		connection_fd = src.connection_fd;
		kernel_q_fd = src.kernel_q_fd;
		response_ready = src.response_ready;
	}
	return (*this);
}

ConnectionHandler::ConnectionHandler(int fd, std::vector<t_server_config> server_config, int q_fd)
{
	connection_fd = fd;
	kernel_q_fd = q_fd;
	response_ready = 0;
	this->server_config = server_config;
	handler = NULL;
}

ConnectionHandler::~ConnectionHandler()
{
	if (handler != NULL)
		delete handler;
}

RequestHandler*	ConnectionHandler::getRequestHandler()
{
	return (handler);
}

void	ConnectionHandler::initRequestHandler()
{
	handler = new RequestHandler(connection_fd, server_config, kernel_q_fd);
}

int	ConnectionHandler::getResponseStatus()
{
	return (response_ready);
}

void	ConnectionHandler::setResponseStatus(bool response_status)
{
	response_ready = response_status;
}

void	ConnectionHandler::removeRequestHandler()
{
	if (handler != NULL)
		delete handler;
	handler = NULL;
}
