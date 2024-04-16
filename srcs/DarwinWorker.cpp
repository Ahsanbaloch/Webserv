
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
		ConnectionHandler* Handler = new ConnectionHandler(pending_fds[i], socket.getServerConfig());
		connected_clients.insert(std::pair<int, ConnectionHandler*>(pending_fds[i], Handler));
	}
}

void	DarwinWorker::closeConnection(int connection_id)
{
	connected_clients[event_lst[connection_id].ident]->removeRequestHandler();
	connected_clients[event_lst[connection_id].ident]->setResponseStatus(0);
	delete connected_clients[event_lst[connection_id].ident];
	close(event_lst[connection_id].ident); // event_lst[i].ident is the file descriptor of the socket that triggered
	connected_clients.erase(event_lst[connection_id].ident);
}


///////// MAIN METHOD ///////////

void	DarwinWorker::runEventLoop()
{
	while (1)
	{
		// check for new events that are registered in our kqueue (could come from a listening or connection socket)
		int new_events = kevent(Q.getKQueueFD(), NULL, 0, event_lst, MAX_EVENTS, NULL); // it depends on several kernel-internal factors whether kevent returns one or multiple events for several conncetion requests. That's why ideally one makes acception checks in a loop per each event
		if (new_events == -1)
			throw CustomException("Failed when checking for new events\n");

		// go through all the events we have been notified of
		for (int i = 0; new_events > i; i++)
		{
			// when client disconnected an EOF is sent. Close fd to rm event from kqueue
			if (event_lst[i].flags & EV_EOF)
			{
				std::cout << "client disconnected\n";
				closeConnection(i);
			}
			// event came from listening socket --> we have to create a connection
			else if (*reinterpret_cast<int*>(event_lst[i].udata) == Q.getListeningSocketIdent())
			{
				while (1) // to improve efficiency (reducing calls to kevent), we accept all connection requests related to the event in a loop
				{
					std::cout << "new connection incoming\n";
					int connection_fd = accept(event_lst[i].ident, (struct sockaddr *)&client_addr, &addr_size); // accept performs the 3-way TCP handshake
					if (connection_fd == -1) // if we have handled all connection requests related to the specific event notification
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
						{
							// need to free memory when fails?
							Q.attachConnectionSockets(pending_fds);
							std::map<int, ListeningSocket>::iterator it = listening_sockets.find(event_lst[i].ident);
							if (it != listening_sockets.end())
								addToConnectedClients(it->second);
							else
								throw CustomException("Failed when connecting clients\n");
							pending_fds.clear();
							break;
						}
						else
							throw CustomException("Failed when trying to establish connection\n");
					}
					pending_fds.push_back(connection_fd);
				}
			}
			// event came from connection, so that we want to handle the request
			else if (*reinterpret_cast<int*>(event_lst[i].udata) == Q.getConnectionSocketIdent())
			{
				if (event_lst[i].filter == EVFILT_READ)
				{
					if (connected_clients[event_lst[i].ident]->getRequestHandler() == NULL)
						connected_clients[event_lst[i].ident]->initRequestHandler();
					connected_clients[event_lst[i].ident]->getRequestHandler()->processRequest();
					connected_clients[event_lst[i].ident]->setResponseStatus(connected_clients[event_lst[i].ident]->getRequestHandler()->getResponseStatus());
				}
				else if (connected_clients[event_lst[i].ident]->getResponseStatus() && event_lst[i].filter == EVFILT_WRITE) // how to provide the reponse_ready info? // should this be an "If" OR "Else if"?
				{
					connected_clients[event_lst[i].ident]->getRequestHandler()->sendResponse();
					if (connected_clients[event_lst[i].ident]->getRequestHandler()->getHeaderInfo().getHeaderFields()["connection"] == "close"
						|| connected_clients[event_lst[i].ident]->getRequestHandler()->getStatus() >= 400)
					{
						std::cout << "disconnected by server\n";
						closeConnection(i);
					}
					else
					{
						connected_clients[event_lst[i].ident]->removeRequestHandler();
						connected_clients[event_lst[i].ident]->setResponseStatus(0);
					}
				}
			}
		}
	}
}
