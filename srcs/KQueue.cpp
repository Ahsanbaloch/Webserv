
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

void	KQueue::attachListeningSockets(ListeningSocketsBlock& SocketsBlock)
{
	// define what events we are interested in (in case of the listening socket we are only interested in the EVFILT_READ
	// since it is only used for accepting incoming connections)
	struct kevent* listening_event = new struct kevent[SocketsBlock.num_listening_sockets];
	int i = 0;
	for (std::map<int, ListeningSocket>::iterator it = SocketsBlock.listening_sockets.begin(); it != SocketsBlock.listening_sockets.end(); it++)
		EV_SET(&listening_event[i++], it->first, EVFILT_READ, EV_ADD, 0, 0, &listening_sock_ident);
	if (kevent(kqueue_fd, listening_event, SocketsBlock.num_listening_sockets, NULL, 0, NULL) == -1)
	{
		perror("Failure: ");
		delete[] listening_event;
		throw CustomException("Failed when registering events for listening sockets\n");
	}
	delete[] listening_event;
}

void	KQueue::attachConnectionSockets(std::vector<int> pending_fds)
{
	int size = pending_fds.size();
	struct kevent* connection_event = new struct kevent[size * 2];
	int j = 0;
	for (int i = 0; i < size; i++)
	{
		ListeningSocket::setNonblocking(pending_fds[i]);
		EV_SET(&connection_event[j++], pending_fds[i], EVFILT_READ, EV_ADD, 0, 0, &connection_sock_ident);
		EV_SET(&connection_event[j++], pending_fds[i], EVFILT_WRITE, EV_ADD, 0, 0, &connection_sock_ident);
	}
	if (kevent(kqueue_fd, connection_event, size * 2, NULL, 0, NULL) < 0)
	{
		delete[] connection_event;
		throw CustomException("Failed when registering events for conncetion sockets\n");
	}
	delete[] connection_event;
}

void	KQueue::closeQueue()
{
	close(kqueue_fd);
}
