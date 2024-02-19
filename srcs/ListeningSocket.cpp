
#include "../includes/ListeningSocket.h"

ListeningSocket::ListeningSocket(int fd)
{
	socket_fd = fd;
	memset(&sock_config, 0, sizeof(sock_config)); // allowed in subject? // also do for other structures?
}

ListeningSocket::~ListeningSocket()
{
}

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

void	ListeningSocket::initSockConfig(int port, u_int32_t ip)
{
	(void)ip;
	sock_config.sin_family = AF_INET;
	sock_config.sin_port = htons(port);
	sock_config.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

void	ListeningSocket::bindSock()
{
	if (bind(socket_fd, (struct sockaddr*)&sock_config, sizeof(sock_config)) < 0)
		throw CustomException("Failed when calling bind()\n");
}

int	ListeningSocket::getSocketFd() const
{
	return (socket_fd);
}

void	ListeningSocket::setNonblocking(int fd)
{
	// the correct way to make the fd non-blocking would be to first get the current flags with F_GETFL and then add the non-blocking one. However, F_GETFL is not allowed
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		throw CustomException("Failed when calling fcntl() and setting fds to non-blocking\n");
}

void	ListeningSocket::makeListen()
{
	setNonblocking(socket_fd);
	if (listen(socket_fd, SOMAXCONN) < 0)
		throw CustomException("Failed when making sockets listening\n");
}
