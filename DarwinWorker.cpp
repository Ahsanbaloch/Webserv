
#include "DarwinWorker.h"
#include "KQueue.h"


DarwinWorker::DarwinWorker(KQueue Queue)
{
	Q = Queue;
}

DarwinWorker::~DarwinWorker()
{
}

// int	DarwinWorker::addConnectionToKernelQueue(std::vector<int> pending_fds)
// {
// 	int size = pending_fds.size();
// 	struct kevent connection_event[size];
// 	for (int i = 0; i < size; i++)
// 	{
// 		setNonblocking(pending_fds[i]);
// 		EV_SET(&connection_event[i], pending_fds[i], EVFILT_READ, EV_ADD, 0, 0, &Q.connection_sock_ident);
// 	}
// 	if (kevent(Q.kqueue_fd, connection_event, size, NULL, 0, NULL) < 0)
// 		return (perror("Failure when registering event"), 1);
// 	return (0);
// }

//function to run event loop
int	DarwinWorker::runEventLoop()
{
	std::vector<int> pending_fds;
	struct sockaddr_storage client_addr;
	socklen_t addr_size;

	while (1)
	{
		// check for new events that are registered in our kqueue (could come from a listening or connection socket)
		int new_events = kevent(Q.kqueue_fd, NULL, 0, event_lst, MAX_EVENTS, NULL); // it depends on several kernel-internal factors whether kevent returns one or multiple events for several conncetion requests. That's why ideally one makes acception checks in a loop per each event
		if (new_events == -1)
			return (perror("Failure when checking for new events"), 1);
		
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
							// addConnectionToKernelQueue(pending_fds);
							pending_fds.clear();
							break;
						}
						else
							return (perror("Failure when trying to establish connection"), 1);
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

