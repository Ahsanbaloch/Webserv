
#include "DarwinWorker.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

DarwinWorker::DarwinWorker()
{
	memset(&client_addr, 0, sizeof(client_addr));
	addr_size = sizeof(client_addr);
}

DarwinWorker::DarwinWorker(const KQueue& Queue, ListeningSocketsBlock& Block)
	: Q(Queue)
{
	listening_sockets = Block.getListeningSockets();
	memset(&client_addr, 0, sizeof(client_addr));
	addr_size = sizeof(client_addr);
}

DarwinWorker::~DarwinWorker()
{
	for (std::map<int, ConnectionHandler*>::iterator it = connected_clients.begin(); it != connected_clients.end(); it++)
	{
		close(it->first);
		delete it->second;
	}
	connected_clients.erase(connected_clients.begin(), connected_clients.end());
}

DarwinWorker::DarwinWorker(const DarwinWorker& src)
{
	Q = src.Q;
	listening_sockets = src.listening_sockets;
	connected_clients = src.connected_clients;
	pending_fds = src.pending_fds;
	client_addr = src.client_addr;
	addr_size = src.addr_size;
	memcpy(event_lst, src.event_lst, sizeof(event_lst));
}

DarwinWorker& DarwinWorker::operator=(const DarwinWorker& src)
{
	if (this != &src)
	{
		Q = src.Q;
		listening_sockets = src.listening_sockets;
		connected_clients = src.connected_clients;
		pending_fds = src.pending_fds;
		client_addr = src.client_addr;
		addr_size = src.addr_size;
		memcpy(event_lst, src.event_lst, sizeof(event_lst));
	}
	return (*this);
}


///////// HELPER METHODS ///////////

void	DarwinWorker::addToConnectedClients(ListeningSocket& socket)
{
	int size = pending_fds.size();
	for (int i = 0; i < size; i++)
	{
		ConnectionHandler* Handler = new ConnectionHandler(pending_fds[i], socket.getServerConfig(), Q.getKQueueFD());
		connected_clients.insert(std::pair<int, ConnectionHandler*>(pending_fds[i], Handler));
	}
}

void	DarwinWorker::closeConnection(int ev_connect_fd)
{
	if (connected_clients.find(ev_connect_fd) == connected_clients.end())
		return ;
	connected_clients[ev_connect_fd]->removeRequestHandler();
	delete connected_clients[ev_connect_fd];
	connected_clients.erase(ev_connect_fd);
	struct kevent ev;
	EV_SET(&ev, ev_connect_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(Q.getKQueueFD(), &ev, 1, NULL, 0, NULL) == -1)
		throw CustomException("Failed when deleting read event");
	EV_SET(&ev, ev_connect_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	if (kevent(Q.getKQueueFD(), &ev, 1, NULL, 0, NULL) == -1)
		throw CustomException("Failed when deleting write event");
	close(ev_connect_fd);
	std::cout << "Connection " << ev_connect_fd << " has been closed" << std::endl;
}

void	DarwinWorker::acceptConnections(int ev_listening_fd)
{
	while (1)
	{
		int connection_fd = accept(ev_listening_fd, (struct sockaddr *)&client_addr, &addr_size);
		if (connection_fd == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				Q.attachConnectionSockets(pending_fds);
				std::map<int, ListeningSocket>::iterator it = listening_sockets.find(ev_listening_fd);
				if (it != listening_sockets.end())
					addToConnectedClients(it->second);
				else
					throw CustomException("Failed when connecting clients");
				pending_fds.clear();
				break;
			}
			else
				throw CustomException("Failed when trying to establish connection");
		}
		std::cout << "Connection " << connection_fd << " has been established" << std::endl;
		pending_fds.push_back(connection_fd);
	}
}

void	DarwinWorker::handleReadEvent(int ev_connect_fd)
{
	try
	{
		if (connected_clients[ev_connect_fd]->getRequestHandler() == NULL)
			connected_clients[ev_connect_fd]->initRequestHandler();
		connected_clients[ev_connect_fd]->getRequestHandler()->processRequest();
		connected_clients[ev_connect_fd]->setResponseStatus(connected_clients[ev_connect_fd]->getRequestHandler()->getResponseStatus());
	}
	catch(const std::exception& e)
	{
		closeConnection(ev_connect_fd);
		std::cerr << e.what() << '\n';
	}
}

void	DarwinWorker::handleWriteEvent(int ev_connect_fd)
{
	try
	{
		connected_clients[ev_connect_fd]->getRequestHandler()->sendResponse();
		if (connected_clients[ev_connect_fd]->getRequestHandler()->getNumResponseChunks() == 0 || connected_clients[ev_connect_fd]->getRequestHandler()->getChunksSentCompleteStatus() == 1)
		{
			if (connected_clients[ev_connect_fd]->getRequestHandler()->getHeaderInfo().getHeaderFields()["connection"] == "close"
			|| connected_clients[ev_connect_fd]->getRequestHandler()->getStatus() >= 400)
			{
				closeConnection(ev_connect_fd);
			}
			else
			{
				connected_clients[ev_connect_fd]->setResponseStatus(0);
				connected_clients[ev_connect_fd]->removeRequestHandler();
			}
		}
	}
	catch(const std::exception& e)
	{
		closeConnection(ev_connect_fd);
		std::cerr << e.what() << '\n';
	}
}

void	DarwinWorker::handleCGIResponse(int connect_ev)
{
	int connect_fd = *static_cast<int*>(event_lst[connect_ev].udata); // connection fd related to CGI is transmitted in udata field
	try
	{
		if (connected_clients.find(connect_fd) != connected_clients.end())
		{
			connected_clients[connect_fd]->getRequestHandler()->initCGIResponse();
			connected_clients[connect_fd]->getRequestHandler()->readCGIResponse();
			connected_clients[connect_fd]->setResponseStatus(connected_clients[connect_fd]->getRequestHandler()->getResponseStatus());
		}
	}
	catch(const std::exception& e)
	{
		closeConnection(connect_fd);
		std::cerr << e.what() << '\n';
	}
}


///////// MAIN METHOD ///////////

void	DarwinWorker::runEventLoop()
{
	while (1)
	{
		try
		{
			int new_events = kevent(Q.getKQueueFD(), NULL, 0, event_lst, MAX_EVENTS, NULL);
			if (new_events == -1)
				throw CustomException("Failed when checking for new events");
			for (int connect_ev = 0; new_events > connect_ev; connect_ev++)
			{
				int socket_ident = *static_cast<int*>(event_lst[connect_ev].udata);
				int event_fd = event_lst[connect_ev].ident;
				if (event_lst[connect_ev].flags & EV_EOF && socket_ident == Q.getConnectionSocketIdent())
				{
					if (connected_clients[event_fd] != NULL)
						closeConnection(event_fd);
				}
				else if (socket_ident == Q.getListeningSocketIdent())
					acceptConnections(event_fd);
				else if (socket_ident == Q.getConnectionSocketIdent() && connected_clients.find(event_fd) != connected_clients.end())
				{
					if (event_lst[connect_ev].filter == EVFILT_READ)
						handleReadEvent(event_fd);
					else if (event_lst[connect_ev].filter == EVFILT_WRITE && connected_clients[event_fd]->getResponseStatus()
						&& connected_clients[event_fd]->getRequestHandler() != NULL)
					{
						handleWriteEvent(event_fd);
					}
				}
				else if (socket_ident != Q.getListeningSocketIdent() && socket_ident != Q.getConnectionSocketIdent())
				{
					handleCGIResponse(connect_ev);
				}
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}
