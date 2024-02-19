
#include "ListeningSocketsBlock.h"

ListeningSocketsBlock::ListeningSocketsBlock(std::vector<int> config_info)
	: num_listening_sockets(2)
{
	createSockets(config_info);
}

ListeningSocketsBlock::~ListeningSocketsBlock()
{
}

// function to create server sockets
void	ListeningSocketsBlock::createSockets(std::vector<int> ports_test)
{
	for (std::vector<int>::iterator it = ports_test.begin(); it != ports_test.end(); it++)
	{
		int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1)
			throw std::exception();

		ListeningSocket serverSocket(socket_fd);
		serverSocket.setSockOptions();
		serverSocket.initSockConfig(*it, 0);
		serverSocket.bindSock();
		serverSocket.makeListen();

		// storing all socket data in a vector (at least for now)
		listening_sockets.push_back(serverSocket);
	}
}
void	ListeningSocketsBlock::closeSockets()
{
	for (std::vector<ListeningSocket>::iterator it = listening_sockets.begin(); it != listening_sockets.end(); it++)
	{
		close(it->getSocketFd());
	}
}
