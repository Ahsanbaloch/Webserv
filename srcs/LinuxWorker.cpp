
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

void	LinuxWorker::closeConnection(int ev_connect_fd)
{
	if (connected_clients.find(ev_connect_fd) == connected_clients.end())
		return ;
	connected_clients[ev_connect_fd]->removeRequestHandler();
	delete connected_clients[ev_connect_fd];
	connected_clients.erase(ev_connect_fd);
	epoll_ctl(Q.getEPOLLFD(), EPOLL_CTL_DEL, ev_connect_fd, NULL);
	close(ev_connect_fd);
	std::cout << "Connection " << ev_connect_fd << " has been closed" << std::endl;
}

void	LinuxWorker::acceptConnections(int ev_listening_fd)
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
			{
				perror("fail");
				throw CustomException("Failed when trying to establish connection");
			}
		}
		std::cout << "Connection " << connection_fd << " has been established" << std::endl;
		pending_fds.push_back(connection_fd);
	}
}

void	LinuxWorker::handleReadEvent(int ev_connect_fd)
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

void	LinuxWorker::handleWriteEvent(int ev_connect_fd)
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

void	LinuxWorker::handleCGIResponse(int ev_connect_fd)
{
	try
	{
		if (connected_clients.find(ev_connect_fd) != connected_clients.end())
		{
			connected_clients[ev_connect_fd]->getRequestHandler()->initCGIResponse();
			connected_clients[ev_connect_fd]->getRequestHandler()->readCGIResponse();
			connected_clients[ev_connect_fd]->setResponseStatus(connected_clients[ev_connect_fd]->getRequestHandler()->getResponseStatus());
		}
	}
	catch(const std::exception& e)
	{
		closeConnection(ev_connect_fd);
		std::cerr << e.what() << '\n';
	}
}

///////// MAIN METHOD ///////////

void	LinuxWorker::runEventLoop()
{
	while (1)
	{
		try
		{
			int new_events = epoll_wait(Q.getEPOLLFD(), event_lst, MAX_EVENTS, -1);
			if (new_events == -1)
				throw CustomException("Failed when checking for new events");
			for (int connect_ev = 0; new_events > connect_ev; connect_ev++)
			{
				int socket_ident = static_cast<EPoll::e_data*>(event_lst[connect_ev].data.ptr)->socket_ident;
				int event_fd = static_cast<EPoll::e_data*>(event_lst[connect_ev].data.ptr)->fd;
				if (socket_ident == Q.getConnectionSockIdent() && ((event_lst[connect_ev].events & EPOLLERR)
					|| (event_lst[connect_ev].events & EPOLLHUP) || (event_lst[connect_ev].events & EPOLLRDHUP)))
				{
					closeConnection(event_fd);
				}
				else if (socket_ident == Q.getListeningSockIdent())
				{
					acceptConnections(event_fd);
				}
				else if (socket_ident == Q.getConnectionSockIdent() && connected_clients.find(event_fd) != connected_clients.end())
				{
					
					if (event_lst[connect_ev].events & EPOLLIN)
					{
						handleReadEvent(event_fd);
					}
					if (event_lst[connect_ev].events & EPOLLOUT && connected_clients.find(event_fd) != connected_clients.end()
						&& connected_clients[event_fd]->getRequestHandler() != NULL && connected_clients[event_fd]->getResponseStatus())
					{
						handleWriteEvent(event_fd);
					}
				}
				else if (socket_ident != Q.getListeningSockIdent() && socket_ident != Q.getConnectionSockIdent())
				{
					handleCGIResponse(event_fd);
				}
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}
