
#include "ListeningSocket.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

ListeningSocket::ListeningSocket()
{
	socket_fd = -1;
	memset(&sock_config, 0, sizeof(sock_config));
}

ListeningSocket::ListeningSocket(int fd, std::vector<t_server_config> config_info)
{
	socket_fd = fd;
	server_config = config_info;
	memset(&sock_config, 0, sizeof(sock_config));
}

ListeningSocket::~ListeningSocket()
{
}

ListeningSocket::ListeningSocket(const ListeningSocket& src)
{
	socket_fd = src.socket_fd;
	server_config = src.server_config;
	memcpy(&sock_config, &src.sock_config, sizeof(sock_config));
}

ListeningSocket& ListeningSocket::operator=(const ListeningSocket& src)
{
	if (this != &src)
	{
		socket_fd = src.socket_fd;
		server_config = src.server_config;
		memcpy(&sock_config, &src.sock_config, sizeof(sock_config));
	}
	return (*this);
}


///////// GETTERS ///////////

std::vector<t_server_config>	ListeningSocket::getServerConfig() const
{
	return (server_config);
}


///////// METHODS ///////////

void	ListeningSocket::setSockOptions()
{
	// set socket options
	// enabling SO_REUSEADDR for the socket indicates that the socket can be reused even if it is in a TIME_WAIT state. This can be helpful in scenarios where you want to restart a server quickly after it has been shut down, without waiting for the operating system to release the socket.
	// A TCP local socket address that has been bound is unavailable for some time after closing, unless the SO_REUSEADDR flag has been
	// set.  Care should be taken when using this flag as it makes TCP less reliable.
	// how is it related to timeout functionality (RFC 9112 (HTTP 1.1) section 9.5) and do we implement it?
	int enable = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
		throw CustomException("Failed when calling setsocketopt()\n");
}

void	ListeningSocket::initSockConfig()
{
	sock_config.sin_family = AF_INET;
	sock_config.sin_port = htons(server_config[0].port);
	sock_config.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

void	ListeningSocket::bindSock()
{
	if (bind(socket_fd, (struct sockaddr*)&sock_config, sizeof(sock_config)) < 0)
	{
		throw CustomException("Failed when calling bind()\n");
	}
}

int	ListeningSocket::getSocketFd() const
{
	return (socket_fd);
}

void	ListeningSocket::setNonblocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		throw CustomException("Failed when calling fcntl() and setting fds to non-blocking\n");
}

void	ListeningSocket::makeListen()
{
	if (listen(socket_fd, SOMAXCONN) < 0)
		throw CustomException("Failed when making sockets listening\n");
}
