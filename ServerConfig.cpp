
#include "ServerConfig.h"

ServerConfig::ServerConfig()
	: num_listening_sockets(2)
{
	listening_sockets = createSockets();
}

ServerConfig::~ServerConfig()
{
}

// function to create server socket
std::vector<ListeningSocket>	ServerConfig::createSockets()
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

		// storing all socket data in a vector (at least for now)
		listening_sockets.push_back(serverSocket);
	}
	return (listening_sockets);
}

void	ServerConfig::setNonblocking(int fd)
{
	// the correct way to make the fd non-blocking would be to first get the current flags with F_GETFL and then add the non-blocking one. However, F_GETFL is not allowed
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		perror("fcntl failure"); // is perror allowed?
}

// may add to ListeningSocket class instead?
void	ServerConfig::listen2()
{
	for (std::vector<ListeningSocket>::iterator it = listening_sockets.begin(); it != listening_sockets.end(); it++)
	{
		setNonblocking(it->getSocketFd());
		if (listen(it->getSocketFd(), SOMAXCONN) < 0)
			perror("Failure when listening for requests");
	}
}

void	ServerConfig::closeSockets()
{
	for (std::vector<ListeningSocket>::iterator it = listening_sockets.begin(); it != listening_sockets.end(); it++)
	{
		close(it->getSocketFd());
	}
}
