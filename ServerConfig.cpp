
#include "ServerConfig.h"

ServerConfig::ServerConfig()
	: num_listening_sockets(2)
{
	listening_sockets = createSockets();
}

ServerConfig::~ServerConfig()
{
}
