
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

int	KQueue::attachListeningSockets(ServerConfig Server)
{
	// define what events we are interested in (in case of the listening socket we are only interested in the EVFILT_READ
	// since it is only used for accepting incoming connections)
	struct kevent listening_event[Server.num_listening_sockets];
	// struct addrinfo *listening_sock_ident;
	for (int i = 0; i < Server.num_listening_sockets; i++)
		EV_SET(&listening_event[i], Server.listening_sockets[i].getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, &listening_sock_ident);
	if (kevent(kqueue_fd, listening_event, Server.num_listening_sockets, NULL, 0, NULL) == -1)
		return (perror("Failure in registering event"), 1);
	return (0);
}

int KQueue::attachConnectionSockets(std::vector<int> pending_fds)
{
	int size = pending_fds.size();
	struct kevent connection_event[size];
	for (int i = 0; i < size; i++)
	{
		ServerConfig::setNonblocking(pending_fds[i]);
		EV_SET(&connection_event[i], pending_fds[i], EVFILT_READ, EV_ADD, 0, 0, &connection_sock_ident);
	}
	if (kevent(kqueue_fd, connection_event, size, NULL, 0, NULL) < 0)
		return (perror("Failure when registering event"), 1);
	return (0);
}

