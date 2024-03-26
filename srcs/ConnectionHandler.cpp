
#include "ConnectionHandler.h"

ConnectionHandler::ConnectionHandler(/* args */)
{
}

ConnectionHandler::ConnectionHandler(int fd, std::vector<t_server_config> server_config)
{
	connection_fd = fd;
	server_config = this->server_config;
}



ConnectionHandler::~ConnectionHandler()
{
}