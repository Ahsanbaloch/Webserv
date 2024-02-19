
#include "ListeningSocketsBlock.h"

ListeningSocketsBlock::ListeningSocketsBlock()
	: num_listening_sockets(2)
{
	listening_sockets = createSockets();
}

ListeningSocketsBlock::~ListeningSocketsBlock()
{
}

// function to create server socket
std::vector<ListeningSocket>	ListeningSocketsBlock::createSockets()
{
	std::vector<ListeningSocket> listening_sockets;

	// for testing multiple ports --> info incl. ip comes from config file
	std::vector<int> ports_test;
	ports_test.push_back(4242);
	ports_test.push_back(8080);

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
	return (listening_sockets);
}
void	ListeningSocketsBlock::closeSockets()
{
	for (std::vector<ListeningSocket>::iterator it = listening_sockets.begin(); it != listening_sockets.end(); it++)
	{
		close(it->getSocketFd());
	}
}
