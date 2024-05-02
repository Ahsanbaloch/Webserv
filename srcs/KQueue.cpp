
#include "KQueue.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

KQueue::KQueue()
{
	kqueue_fd = kqueue();
	if (kqueue_fd == -1)
		throw std::exception();
	listening_sock_ident = 1;
	connection_sock_ident = 2;
}

KQueue::~KQueue()
{
}

KQueue::KQueue(const KQueue& src)
{
	kqueue_fd = src.kqueue_fd;
	if (kqueue_fd == -1)
		throw std::exception();
	listening_sock_ident = src.listening_sock_ident;
	connection_sock_ident = src.connection_sock_ident;
}

KQueue& KQueue::operator=(const KQueue& src)
{
	if (this != &src)
	{
		kqueue_fd = src.kqueue_fd;
		if (kqueue_fd == -1)
			throw std::exception();
		listening_sock_ident = src.listening_sock_ident;
		connection_sock_ident = src.connection_sock_ident;
	}
	return (*this);
}


///////// GETTERS ///////////

int	KQueue::getKQueueFD() const
{
	return (kqueue_fd);
}

int	KQueue::getListeningSocketIdent() const
{
	return (listening_sock_ident);
}

int	KQueue::getConnectionSocketIdent() const
{
	return (connection_sock_ident);
}


///////// METHODS ///////////

void	KQueue::attachListeningSockets(ListeningSocketsBlock& SocketsBlock)
{
	struct kevent* listening_event = new struct kevent[SocketsBlock.getNumListeningSockets()];
	int i = 0;

	std::map<int, ListeningSocket> listening_sockets = SocketsBlock.getListeningSockets();
	for (std::map<int, ListeningSocket>::iterator it = listening_sockets.begin(); it != listening_sockets.end(); it++)
	{
		EV_SET(&listening_event[i++], it->first, EVFILT_READ, EV_ADD, 0, 0, &listening_sock_ident);
		std::cout << "Localhost listening on port " << it->second.getServerConfig()[0].port << " ..." << std::endl;
	}
	if (kevent(kqueue_fd, listening_event, SocketsBlock.getNumListeningSockets(), NULL, 0, NULL) == -1)
	{
		delete[] listening_event;
		throw CustomException("Failed when registering events for listening sockets");
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
		throw CustomException("Failed when registering events for connection sockets");
	}
	delete[] connection_event;
}

void	KQueue::closeQueue()
{
	close(kqueue_fd);
}
