
#include "../includes/DarwinWorker.h"

DarwinWorker::DarwinWorker(const KQueue& Queue)
	: Q(Queue)
{
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
				close(event_lst[i].ident); // event_lst[i].ident is the file descriptor of the socket that triggered
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
							pending_fds.clear();
							break;
						}
						else
							throw CustomException("Failed when trying to establish connection\n");
					}
					pending_fds.push_back(connection_fd);
				}
			}
			// event came from conncetion, so that we want to handle the request
			else if (*reinterpret_cast<int*>(event_lst[i].udata) == Q.connection_sock_ident)
				RequestHandler.handleRequest(event_lst[i]);
		}
	}
}

