
#include "KQueue.h"

KQueue::KQueue(/* args */)
{
	kqueue_fd = kqueue();
	if (kqueue_fd == -1)
		throw std::exception();
	listening_sock_ident = 0;
	connection_sock_ident = 1;
}

KQueue::~KQueue()
{
}

void	KQueue::attachListeningSockets(const ListeningSocketsBlock& SocketsBlock)
{
	// define what events we are interested in (in case of the listening socket we are only interested in the EVFILT_READ
	// since it is only used for accepting incoming connections)
	struct kevent listening_event[SocketsBlock.num_listening_sockets];
	// struct addrinfo *listening_sock_ident;
	for (int i = 0; i < SocketsBlock.num_listening_sockets; i++)
		EV_SET(&listening_event[i], SocketsBlock.listening_sockets[i].getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, &listening_sock_ident);
	if (kevent(kqueue_fd, listening_event, SocketsBlock.num_listening_sockets, NULL, 0, NULL) == -1)
		throw CustomException("Failed when registering events for listening sockets\n");
}

void	KQueue::attachConnectionSockets(std::vector<int> pending_fds)
{
	int size = pending_fds.size();
	struct kevent connection_event[size];
	for (int i = 0; i < size; i++)
	{
		ListeningSocket::setNonblocking(pending_fds[i]);
		EV_SET(&connection_event[i], pending_fds[i], EVFILT_READ, EV_ADD, 0, 0, &connection_sock_ident);
	}
	if (kevent(kqueue_fd, connection_event, size, NULL, 0, NULL) < 0)
		throw CustomException("Failed when registering events for conncetion sockets\n");
}

void	KQueue::closeQueue()
{
	close(kqueue_fd);
}
