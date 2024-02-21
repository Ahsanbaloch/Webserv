
#include "../includes/EPoll.h"

EPoll::EPoll(const ListeningSocketsBlock& Sockets)
{
	SocketsBlock = Sockets;
	epoll_fd = epoll_create(1);
	if (epoll_fd == -1)
		throw std::exception();
	// listening_sock_ident = 0;
	// connection_sock_ident = 1;
}

EPoll::~EPoll()
{
}

void	EPoll::attachListeningSockets()
{
	// define what events we are interested in (in case of the listening socket we are only interested in the EVFILT_READ
	// since it is only used for accepting incoming connections)
	for (int i = 0; i < SocketsBlock.num_listening_sockets; i++)
	{
		struct epoll_event listening_event;
		listening_event.data.fd = SocketsBlock.listening_sockets[i].getSocketFd();
		listening_event.events = EPOLLIN;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, SocketsBlock.listening_sockets[i].getSocketFd(), &listening_event) == -1)
			throw CustomException("Failed when registering events for listening sockets\n");
	}
}

void	EPoll::attachConnectionSockets(std::vector<int> pending_fds)
{
	int size = pending_fds.size();
	struct epoll_event connection_event;
	for (int i = 0; i < size; i++)
	{
		ListeningSocket::setNonblocking(pending_fds[i]); // check if this can be done without fcntl for epoll
		connection_event.data.fd = pending_fds[i];
		connection_event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP; // might also use EPOLLOUT
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pending_fds[i], &connection_event) == -1)
			throw CustomException("Failed when registering events for conncetion sockets\n");
	}
}

void	EPoll::closeQueue()
{
	close(epoll_fd);
}
