
#include "EPoll.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

EPoll::EPoll()
{
	epoll_fd = epoll_create(1);
	if (epoll_fd == -1)
		throw std::exception();
	listening_sock_ident = 1;
	connection_sock_ident = 2;
}

EPoll::~EPoll()
{
}

EPoll::EPoll(const EPoll& src)
{
	epoll_fd = src.epoll_fd;
	if (epoll_fd == -1)
		throw std::exception();
	listening_sock_ident = src.listening_sock_ident;
	connection_sock_ident = src.connection_sock_ident;
}

EPoll& EPoll::operator=(const EPoll& src)
{
	if (this != &src)
	{
		epoll_fd = src.epoll_fd;
		if (epoll_fd == -1)
			throw std::exception();
		listening_sock_ident = src.listening_sock_ident;
		connection_sock_ident = src.connection_sock_ident;
	}
	return (*this);
}


///////// GETTERS ///////////

int	EPoll::getEPOLLFD()
{
	return (epoll_fd);
}

uint32_t	EPoll::getListeningSockIdent() const
{
	return (listening_sock_ident);
}


uint32_t	EPoll::getConnectionSockIdent() const
{
	return (connection_sock_ident);
}


///////// METHODS ///////////

void	EPoll::attachListeningSockets(ListeningSocketsBlock& SocketsBlock)
{
	for (std::map<int, ListeningSocket>::iterator it = SocketsBlock.getListeningSockets().begin(); it != SocketsBlock.getListeningSockets().end(); it++)
	{
		struct epoll_event listening_event;
		listening_event.data.fd = it->first;
		listening_event.data.u32 = listening_sock_ident; 
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
		connection_event.data.u32 = connection_sock_ident;
		connection_event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLOUT; // set EPOLLOUT later?
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pending_fds[i], &connection_event) == -1)
			throw CustomException("Failed when registering events for conncetion sockets\n");
	}
}

void	EPoll::closeQueue()
{
	close(epoll_fd);
}


