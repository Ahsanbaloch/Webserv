
#include "LinuxWorker.h"


///////// CONSTRUCTORS & DESTRUCTORS ///////////

LinuxWorker::LinuxWorker()
{
	memset(&client_addr, 0, sizeof(client_addr));
	addr_size = sizeof(client_addr);
}

LinuxWorker::LinuxWorker(const EPoll& Queue, ListeningSocketsBlock& SocketsBlock)
	: Q(Queue)
{
	listening_sockets = SocketsBlock.getListeningSockets();
	memset(&client_addr, 0, sizeof(client_addr));
	addr_size = sizeof(client_addr);
}

LinuxWorker::~LinuxWorker()
{
}

LinuxWorker::LinuxWorker(const LinuxWorker& src)
{
	Q = src.Q;
	listening_sockets = src.listening_sockets;
	connected_clients = src.connected_clients;
	pending_fds = src.pending_fds;
	client_addr = src.client_addr;
	addr_size = src.addr_size;
	memset(event_lst, 0, sizeof(event_lst));
}

LinuxWorker&	LinuxWorker::operator=(const LinuxWorker& src)
{
	if (this != &src)
	{
		Q = src.Q;
		listening_sockets = src.listening_sockets;
		connected_clients = src.connected_clients;
		pending_fds = src.pending_fds;
		client_addr = src.client_addr;
		addr_size = src.addr_size;
		memset(event_lst, 0, sizeof(event_lst));
	}
	return (*this);
}


///////// HELPER METHODS ///////////

void	LinuxWorker::addToConnectedClients(ListeningSocket& socket)
{
	int size = pending_fds.size();
	for (int i = 0; i < size; i++)
	{
		ConnectionHandler* Handler = new ConnectionHandler(pending_fds[i], socket.getServerConfig(), Q.getEPOLLFD());
		connected_clients.insert(std::pair<int, ConnectionHandler*>(pending_fds[i], Handler));
	}
}

void	LinuxWorker::closeConnection(int connect_ev)
{
	connected_clients[event_lst[connect_ev].data.fd]->removeRequestHandler();
	delete connected_clients[event_lst[connect_ev].data.fd];
	connected_clients.erase(event_lst[connect_ev].data.fd);
	close(event_lst[connect_ev].data.fd);
	std::cout << "Connection " << event_lst[connect_ev].data.fd << " has been closed" << std::endl;
}

void	LinuxWorker::acceptConnections(int connect_ev)
{
	while (1)
	{
		int connection_fd = accept(event_lst[connect_ev].data.fd, (struct sockaddr *)&client_addr, &addr_size);
		if (connection_fd == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				Q.attachConnectionSockets(pending_fds);
				std::map<int, ListeningSocket>::iterator it = listening_sockets.find(event_lst[connect_ev].data.fd);
				if (it != listening_sockets.end())
					addToConnectedClients(it->second);
				else
					throw CustomException("Failed when connecting clients"); // where is this going?
				pending_fds.clear();
				break;
			}
			else
				throw CustomException("Failed when trying to establish connection"); // where is this going?
		}
		std::cout << "Connection " << connection_fd << " has been established" << std::endl;
		pending_fds.push_back(connection_fd);
	}
}

void	LinuxWorker::handleReadEvent(int connect_ev)
{
	try
	{
		if (connected_clients[event_lst[connect_ev].data.fd]->getRequestHandler() == NULL)
			connected_clients[event_lst[connect_ev].data.fd]->initRequestHandler();
		connected_clients[event_lst[connect_ev].data.fd]->getRequestHandler()->processRequest();
		connected_clients[event_lst[connect_ev].data.fd]->setResponseStatus(connected_clients[event_lst[connect_ev].data.fd]->getRequestHandler()->getResponseStatus());
	}
	catch(const std::exception& e)
	{
		closeConnection(connect_ev);
		std::cerr << e.what() << '\n';
	}
}

void	LinuxWorker::handleWriteEvent(int connect_ev)
{
	try
	{
		connected_clients[event_lst[connect_ev].data.fd]->getRequestHandler()->sendResponse();
		if (connected_clients[event_lst[connect_ev].data.fd]->getRequestHandler()->getNumResponseChunks() == 0 || connected_clients[event_lst[connect_ev].data.fd]->getRequestHandler()->getChunksSentCompleteStatus() == 1)
		{
			if (connected_clients[event_lst[connect_ev].data.fd]->getRequestHandler()->getHeaderInfo().getHeaderFields()["connection"] == "close"
			|| connected_clients[event_lst[connect_ev].data.fd]->getRequestHandler()->getStatus() >= 400)
			{
				closeConnection(connect_ev);
			}
			else
			{
				connected_clients[event_lst[connect_ev].data.fd]->setResponseStatus(0);
				connected_clients[event_lst[connect_ev].data.fd]->removeRequestHandler();
			}
		}
	}
	catch(const std::exception& e)
	{
		closeConnection(connect_ev);
		std::cerr << e.what() << '\n';
	}
}

void	LinuxWorker::handleCGIResponse(int connect_ev)
{
	try
	{
		connected_clients[event_lst[connect_ev].data.u64]->getRequestHandler()->initCGIResponse();
		connected_clients[event_lst[connect_ev].data.u64]->getRequestHandler()->readCGIResponse();
		connected_clients[event_lst[connect_ev].data.u64]->setResponseStatus(connected_clients[event_lst[connect_ev].data.u64]->getRequestHandler()->getResponseStatus());
	}
	catch(const std::exception& e)
	{
		closeConnection(connect_ev);
		std::cerr << e.what() << '\n';
	}
}

///////// MAIN METHOD ///////////

void	LinuxWorker::runEventLoop()
{
	while (1)
	{
		int new_events = epoll_wait(Q.getEPOLLFD(), event_lst, MAX_EVENTS, -1);
		if (new_events == -1)
			throw CustomException("Failed when checking for new events");
		
		for (int connect_ev = 0; new_events > connect_ev; connect_ev++)
		{
			// double check what this is exactly checking / epoll error?
			if ((event_lst[connect_ev].events & EPOLLERR)
			    || (event_lst[connect_ev].events & EPOLLHUP)
				|| (event_lst[connect_ev].events & EPOLLRDHUP)
				|| (!(event_lst[connect_ev].events & EPOLLIN)))
			{
				if (connected_clients[event_lst[connect_ev].data.fd] != NULL)
					closeConnection(connect_ev);
			}
			else if (event_lst[connect_ev].data.u32 == Q.getListeningSockIdent())
			{
				acceptConnections(connect_ev);
			}
			else if (event_lst[connect_ev].data.u32 == Q.getConnectionSockIdent() && connected_clients[event_lst[connect_ev].data.fd] != NULL)
			{
				if (event_lst[connect_ev].events & EPOLLIN)
					handleReadEvent(connect_ev);
				else if (connected_clients[event_lst[connect_ev].data.fd]->getRequestHandler() != NULL
					&& connected_clients[event_lst[connect_ev].data.fd]->getResponseStatus() && event_lst[connect_ev].events & EPOLLOUT)
				{
					handleWriteEvent(connect_ev);
				}
			}
			else if(event_lst[connect_ev].data.u32 != Q.getListeningSockIdent() && event_lst[connect_ev].data.u32 == Q.getConnectionSockIdent())
			{
				handleCGIResponse(connect_ev);
			}
		}
	}
}

