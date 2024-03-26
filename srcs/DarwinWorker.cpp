
#include "DarwinWorker.h"

DarwinWorker::DarwinWorker(const KQueue& Queue, ListeningSocketsBlock& Block)
	: Q(Queue)
{
	// SocketsBlock = Block;
	listening_sockets = Block.listening_sockets;
	memset(&client_addr, 0, sizeof(client_addr));
	addr_size = sizeof(client_addr);
}

DarwinWorker::~DarwinWorker()
{
}

//function to run event loop
void	DarwinWorker::runEventLoop()
{
	while (1)
	{
		// check for new events that are registered in our kqueue (could come from a listening or connection socket)
		int new_events = kevent(Q.kqueue_fd, NULL, 0, event_lst, MAX_EVENTS, NULL); // it depends on several kernel-internal factors whether kevent returns one or multiple events for several conncetion requests. That's why ideally one makes acception checks in a loop per each event
		if (new_events == -1)
			throw CustomException("Failed when checking for new events\n");

		// go through all the events we have been notified of
		for (int i = 0; new_events > i; i++)
		{
			// when client disconnected an EOF is sent. Close fd to rm event from kqueue
			if (event_lst[i].flags & EV_EOF)
			{
				std::cout << "client disconnected\n";
				delete ConnectedClients[event_lst[i].ident];
				close(event_lst[i].ident); // event_lst[i].ident is the file descriptor of the socket that triggered
				ConnectedClients.erase(event_lst[i].ident);
			}
			// event came from listening socket --> we have to create a connection
			else if (*reinterpret_cast<int*>(event_lst[i].udata) == Q.listening_sock_ident)
			{
				while (1) // to improve efficiency (reducing calls to kevent), we accept all connection requests related to the event in a loop
				{
					std::cout << "new connection incoming\n";
					int connection_fd = accept(event_lst[i].ident, (struct sockaddr *)&client_addr, &addr_size); // accept performs the 3-way TCP handshake
					if (connection_fd == -1) // if we have handled all connection requests related to the specific event notification
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
						{
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
			else if (*reinterpret_cast<int*>(event_lst[i].udata) == Q.connection_sock_ident)
			{
				if (event_lst[i].filter == EVFILT_READ)
				{
					if (ConnectedClients[event_lst[i].ident]->getRequestHandler() == NULL)
						ConnectedClients[event_lst[i].ident]->initRequestHandler();
					ConnectedClients[event_lst[i].ident]->getRequestHandler()->processRequest();
					ConnectedClients[event_lst[i].ident]->setResponseStatus(ConnectedClients[event_lst[i].ident]->getRequestHandler()->response_ready);
					// ConnectedClients[event_lst[i].ident]->processRequest();
				}
				// else if (ConnectedClients[event_lst[i].ident]->response_ready && event_lst[i].filter == EVFILT_WRITE) // how to provide the reponse_ready info? // should this be an "If" OR "Else if"?
				else if (ConnectedClients[event_lst[i].ident]->getResponseStatus() && event_lst[i].filter == EVFILT_WRITE) // how to provide the reponse_ready info? // should this be an "If" OR "Else if"?
				{
					// ConnectedClients[event_lst[i].ident]->sendResponse();
					ConnectedClients[event_lst[i].ident]->getRequestHandler()->sendResponse();
					// what about the connection header with value "keep-alive" and there is no error?
						// --> probably need to reset all values for the requestHandler class otherwise the while loop exits (response_ready is still true)
					ConnectedClients[event_lst[i].ident]->removeRequestHandler();
					ConnectedClients[event_lst[i].ident]->setResponseStatus(0);
					// close connection if "keep-alive header is not set" // also close connection if an error response was sent?
					// delete ConnectedClients[event_lst[i].ident];
					// close(event_lst[i].ident); // close connection; how does it work with 100-continue response? 
					// ConnectedClients.erase(event_lst[i].ident);
				}
			}
		}
	}
}

void	DarwinWorker::addToConnectedClients(ListeningSocket& socket)
{
	int size = pending_fds.size();
	for (int i = 0; i < size; i++)
	{
		// construct handler with socket/configData as input (maybe reference?)
		ConnectionHandler* Handler = new ConnectionHandler(pending_fds[i], socket.server_config);
		ConnectedClients.insert(std::pair<int, ConnectionHandler*>(pending_fds[i], Handler));
		// RequestHandler* Handler = new RequestHandler(pending_fds[i], socket.server_config); // need to free that memory somewhere --> when disconnecting the client
		// ConnectedClients.insert(std::pair<int, RequestHandler*>(pending_fds[i], Handler));
	}
}


