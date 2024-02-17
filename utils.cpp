
#include "utils.h"


// function to set fd as nonblocking // could also be done with open() call
void	setNonblocking(int fd) // --> do when doing socket creation and socketopt
{
	// the correct way to make the fd non-blocking would be to first get the current flags with F_GETFL and then add the non-blocking one. However, F_GETFL is not allowed
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		perror("fcntl failure"); // is perror allowed?
}

// function to create server socket
std::vector<ListeningSocket>	createSockets()
{
	std::vector<ListeningSocket> listening_sockets;

	// for testing multiple ports --> info incl. ip comes from config file
	std::vector<int> ports_test;
	ports_test.push_back(4242);
	ports_test.push_back(8080);

	for (std::vector<int>::iterator it = ports_test.begin(); it != ports_test.end(); it++)
	{
		int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1)
			throw std::exception();

		ListeningSocket serverSocket(socket_fd);
		serverSocket.setSockOptions();
		serverSocket.initSockConfig(*it, 0);
		serverSocket.bindSock();

		// storing all socket data in a vector (at least for now)
		listening_sockets.push_back(serverSocket);
	}
	return (listening_sockets);
}

// read request handler
void	handleRequest(struct kevent event_lst_item)
{
	char buf[BUFFER_SIZE];
	if (event_lst_item.filter == EVFILT_READ)
	{
		size_t bytes_read = recv(event_lst_item.ident, buf, sizeof(buf), 0);
		printf("read %zu bytes\n", bytes_read);	
	}
}

int	addConnectionToKernelQueue(KQueue Queue, std::vector<int> pending_fds, struct addrinfo *connection_sock_ident)
{
	int size = pending_fds.size();
	struct kevent connection_event[size];
	for (int i = 0; i < size; i++)
	{
		setNonblocking(pending_fds[i]);
		EV_SET(&connection_event[i], pending_fds[i], EVFILT_READ, EV_ADD, 0, 0, (void *)connection_sock_ident);
	}
	if (kevent(Queue.kqueue_fd, connection_event, size, NULL, 0, NULL) < 0)
		return (perror("Failure when registering event"), 1);
	return (0);
}

//function to run event loop
int	runEventLoop(KQueue Queue)
{
	std::vector<int> pending_fds;
	struct sockaddr_storage client_addr;
	struct addrinfo *connection_sock_ident;
	socklen_t addr_size;

	while (1)
	{
		struct kevent event_lst[MAX_EVENTS];
		
		// check for new events that are registered in our kqueue (could come from a listening or connection socket)
		int new_events = kevent(Queue.kqueue_fd, NULL, 0, event_lst, MAX_EVENTS, NULL); // it depends on several kernel-internal factors whether kevent returns one or multiple events for several conncetion requests. That's why ideally one makes acception checks in a loop per each event
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
			else if (event_lst[i].udata == Queue.listening_sock_ident)
			{
				while (1) // to improve efficiency (reducing calls to kevent), we accept all connection requests related to the event in a loop
				{
					std::cout << "new connection incoming\n";
					int connection_fd = accept(event_lst[i].ident, (struct sockaddr *)&client_addr, &addr_size); // accept performs the 3-way TCP handshake
					if (connection_fd == -1) // if we have handled all connection requests related to the specific event notification
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
						{
							addConnectionToKernelQueue(Queue, pending_fds, connection_sock_ident);
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
			else if (event_lst[i].udata == connection_sock_ident)
				handleRequest(event_lst[i]);
				
		}
	}
}
