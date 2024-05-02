
#include "ListeningSocketsBlock.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

ListeningSocketsBlock::ListeningSocketsBlock()
{
}

ListeningSocketsBlock::ListeningSocketsBlock(std::map<std::string, std::vector<t_server_config> > &config_info)
	: num_listening_sockets(config_info.size())
{
	createSockets(config_info);
}

ListeningSocketsBlock::~ListeningSocketsBlock()
{
	closeSockets();
}

ListeningSocketsBlock::ListeningSocketsBlock(const ListeningSocketsBlock& src)
	: num_listening_sockets(src.num_listening_sockets), listening_sockets(src.listening_sockets)
{
}


ListeningSocketsBlock& ListeningSocketsBlock::operator=(const ListeningSocketsBlock& src)
{
	if (this != &src)
	{
		num_listening_sockets = src.num_listening_sockets;
		listening_sockets = src.listening_sockets;
	}
	return (*this);
}


///////// GETTERS ///////////

int	ListeningSocketsBlock::getNumListeningSockets() const
{
	return (num_listening_sockets);
}

std::map<int, ListeningSocket> ListeningSocketsBlock::getListeningSockets() const
{
	return (listening_sockets);
}


///////// MAIN METHODS ///////////

void	ListeningSocketsBlock::createSockets(std::map<std::string, std::vector<t_server_config> > &config_info)
{
	for (std::map<std::string, std::vector<t_server_config> >::iterator it = config_info.begin(); it != config_info.end(); it++)
	{
		#ifdef __APPLE__
			int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		#else
			int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		#endif
		if (socket_fd == -1)
			throw CustomException("Failed when calling socket()");

		ListeningSocket serverSocket(socket_fd, it->second);
		serverSocket.setSockOptions();
		serverSocket.initSockConfig();
		serverSocket.bindSock();
		#ifdef __APPLE__
			serverSocket.setNonblocking(serverSocket.getSocketFd());
		#endif
		serverSocket.makeListen();
		listening_sockets.insert(std::pair<int, ListeningSocket> (socket_fd, serverSocket));
	}
}

void	ListeningSocketsBlock::closeSockets()
{
	for (std::map<int, ListeningSocket>::iterator it = listening_sockets.begin(); it != listening_sockets.end(); it++)
	{
		close(it->first);
	}
}
