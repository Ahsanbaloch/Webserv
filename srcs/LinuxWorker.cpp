
#include "LinuxWorker.h"

LinuxWorker::LinuxWorker(const EPoll& Queue)
	: Q(Queue)
{
	memset(&client_addr, 0, sizeof(client_addr));
	addr_size = 0; // shouldn't this be rather be sizeof(client_addr)?
	for (std::vector<ListeningSocket>::iterator it = Q.SocketsBlock.listening_sockets.begin(); it != Q.SocketsBlock.listening_sockets.end(); it++)
		listening_socks_fd.push_back(it->getSocketFd());
}

LinuxWorker::~LinuxWorker()
{
}

void	LinuxWorker::addToConnectedClients()
{
	int size = pending_fds.size();
	for (int i = 0; i < size; i++)
	{
		RequestHandler* Handler = new RequestHandler(pending_fds[i]); // need to free that memory somewhere --> when disconnecting the client
		ConnectedClients.insert(std::pair<int, RequestHandler*>(pending_fds[i], Handler));
	}
}

//function to run event loop
void	LinuxWorker::runEventLoop()
{
	while (1)
	{
		// check for new events that are registered in our kqueue (could come from a listening or connection socket)
		int new_events = epoll_wait(Q.epoll_fd, event_lst, MAX_EVENTS, -1);
		if (new_events == -1)
			throw CustomException("Failed when checking for new events\n");
		
		// go through all the events we have been notified of
		for (int i = 0; new_events > i; i++)
		{
			// double check what this is exactly checking
			if ((event_lst[i].events & EPOLLERR)
			    || (event_lst[i].events & EPOLLHUP)
				|| (event_lst[i].events & EPOLLRDHUP)
				|| (!(event_lst[i].events & EPOLLIN)))
			{
				std::cout << "client disconnected\n"; // or epoll error?
				close(event_lst[i].data.fd);
			}
			// event came from listening socket --> we have to create a connection // maybe there is a different way for checking?
			else if (std::find(listening_socks_fd.begin(), listening_socks_fd.end(), event_lst[i].data.fd) != listening_socks_fd.end())
			{
				while (1) // to improve efficiency (reducing calls to kevent), we accept all connection requests related to the event in a loop
				{
					std::cout << "new connection incoming\n";
					int connection_fd = accept(event_lst[i].data.fd, (struct sockaddr *)&client_addr, &addr_size); // accept performs the 3-way TCP handshake
					if (connection_fd == -1) // if we have handled all connection requests related to the specific event notification
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
						{
							Q.attachConnectionSockets(pending_fds);
							addToConnectedClients();
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
			else
				ConnectedClients[event_lst[i].ident]->handleRequest(event_lst[i].ident); // rm ident in handleRequest and use fd stored in object
		}
	}
}

