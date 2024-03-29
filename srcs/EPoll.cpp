
#include "EPoll.h"

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
	for (std::map<int, ListeningSocket>::iterator it = SocketsBlock.listening_sockets.begin(); it != SocketsBlock.listening_sockets.end(); it++)
	{
		struct epoll_event listening_event;
		listening_event.data.fd = it->first; // needed?
		listening_event.events = EPOLLIN;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, it->first, &listening_event) == -1)
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
