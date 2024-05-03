
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

void	DarwinWorker::closeConnection(int connect_ev)
{
	connected_clients[event_lst[connect_ev].ident]->removeRequestHandler();
	delete connected_clients[event_lst[connect_ev].ident];
	connected_clients.erase(event_lst[connect_ev].ident);
	close(event_lst[connect_ev].ident);
	std::cout << "Connection " << event_lst[connect_ev].ident << " has been closed" << std::endl;
}

void	DarwinWorker::acceptConnections(int connect_ev)
{
	while (1)
	{
		int connection_fd = accept(event_lst[connect_ev].ident, (struct sockaddr *)&client_addr, &addr_size);
		if (connection_fd == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				Q.attachConnectionSockets(pending_fds);
				std::map<int, ListeningSocket>::iterator it = listening_sockets.find(event_lst[connect_ev].ident);
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

void	DarwinWorker::handleReadEvent(int connect_ev)
{
	try
	{
		if (connected_clients[event_lst[connect_ev].ident]->getRequestHandler() == NULL)
			connected_clients[event_lst[connect_ev].ident]->initRequestHandler();
		connected_clients[event_lst[connect_ev].ident]->getRequestHandler()->processRequest();
		connected_clients[event_lst[connect_ev].ident]->setResponseStatus(connected_clients[event_lst[connect_ev].ident]->getRequestHandler()->getResponseStatus());
	}
	catch(const std::exception& e)
	{
		closeConnection(connect_ev);
		std::cerr << e.what() << '\n';
	}
}

void	DarwinWorker::handleWriteEvent(int connect_ev)
{
	try
	{
		connected_clients[event_lst[connect_ev].ident]->getRequestHandler()->sendResponse();
		if (connected_clients[event_lst[connect_ev].ident]->getRequestHandler()->getNumResponseChunks() == 0 || connected_clients[event_lst[connect_ev].ident]->getRequestHandler()->getChunksSentCompleteStatus() == 1)
		{
			if (connected_clients[event_lst[connect_ev].ident]->getRequestHandler()->getHeaderInfo().getHeaderFields()["connection"] == "close"
			|| connected_clients[event_lst[connect_ev].ident]->getRequestHandler()->getStatus() >= 400)
			{
				closeConnection(connect_ev);
			}
			else
			{
				connected_clients[event_lst[connect_ev].ident]->setResponseStatus(0);
				connected_clients[event_lst[connect_ev].ident]->removeRequestHandler();
			}
		}
	}
	catch(const std::exception& e)
	{
		closeConnection(connect_ev);
		std::cerr << e.what() << '\n';
	}
}

void	DarwinWorker::handleCGIResponse(int connect_ev)
{
	try
	{
		if (connected_clients[*static_cast<int*>(event_lst[connect_ev].udata)] != NULL)
		{
			connected_clients[*static_cast<int*>(event_lst[connect_ev].udata)]->getRequestHandler()->initCGIResponse();
			connected_clients[*static_cast<int*>(event_lst[connect_ev].udata)]->getRequestHandler()->readCGIResponse();
			connected_clients[*static_cast<int*>(event_lst[connect_ev].udata)]->setResponseStatus(connected_clients[*static_cast<int*>(event_lst[connect_ev].udata)]->getRequestHandler()->getResponseStatus());
		}
	}
	catch(const std::exception& e)
	{
		closeConnection(*static_cast<int*>(event_lst[connect_ev].udata));
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
				if (event_lst[connect_ev].flags & EV_EOF && *static_cast<int*>(event_lst[connect_ev].udata) == Q.getConnectionSocketIdent())
				{
					if (connected_clients[event_lst[connect_ev].ident] != NULL)
						closeConnection(connect_ev);
				}
				else if (*static_cast<int*>(event_lst[connect_ev].udata) == Q.getListeningSocketIdent())
					acceptConnections(connect_ev);
				else if (*static_cast<int*>(event_lst[connect_ev].udata) == Q.getConnectionSocketIdent()
						&& connected_clients[event_lst[connect_ev].ident] != NULL)
				{
					if (event_lst[connect_ev].filter == EVFILT_READ)
						handleReadEvent(connect_ev);
					else if (event_lst[connect_ev].filter == EVFILT_WRITE && connected_clients[event_lst[connect_ev].ident]->getResponseStatus()
						&& connected_clients[event_lst[connect_ev].ident]->getRequestHandler() != NULL)
					{
						handleWriteEvent(connect_ev);
					}
				}
				else if (*static_cast<int*>(event_lst[connect_ev].udata) != Q.getListeningSocketIdent()
						&& *static_cast<int*>(event_lst[connect_ev].udata) != Q.getConnectionSocketIdent())
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
