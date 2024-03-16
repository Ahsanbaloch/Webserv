
#include "ListeningSocketsBlock.h"

ListeningSocketsBlock::ListeningSocketsBlock(std::map<std::string, std::vector<t_server_config> > &config_info)
	: num_listening_sockets(2)
{
	createSockets(config_info); // here goes the config vector
}

ListeningSocketsBlock::ListeningSocketsBlock()
{
}

ListeningSocketsBlock::~ListeningSocketsBlock()
{
}

// function to create server sockets
void	ListeningSocketsBlock::createSockets(std::map<std::string, std::vector<t_server_config> > &config_info)
{
	for (std::map<std::string, std::vector<t_server_config> >::iterator it = config_info.begin(); it != config_info.end(); it++) // will loop through config vector
	{
		#ifdef __APPLE__
			int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		#else
			int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		#endif
		if (socket_fd == -1)
			throw CustomException("Failed when calling socket()\n");

		ListeningSocket serverSocket(socket_fd);
		serverSocket.setSockOptions();
		serverSocket.initSockConfig(it->second);
		serverSocket.bindSock();
		#ifdef __APPLE__
			serverSocket.setNonblocking(serverSocket.getSocketFd());
		#endif
		serverSocket.makeListen();

		// storing all socket data in a vector (at least for now)
		listening_sockets.push_back(serverSocket);

		// populate socket_fd/configuration map
		server_configs.insert(std::pair<int, std::vector<t_server_config> >(socket_fd, it->second));
	}
}

void	ListeningSocketsBlock::closeSockets()
{
	for (std::vector<ListeningSocket>::iterator it = listening_sockets.begin(); it != listening_sockets.end(); it++)
	{
		close(it->getSocketFd());
	}
}
