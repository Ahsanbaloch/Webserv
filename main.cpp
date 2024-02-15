#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>
#include <map>
#include <vector>

#include "ListeningSocket.hpp"

#define BUFFER_SIZE 1024
#define MAX_EVENTS 128 // how to determine what to set here? --> maybe partly related to SOMAXCONN, but apparently not entirely


// next steps:
// - accept connections in a loop (--> do we need an array for the connection fds?)
// create a listening socket class and then have a vector of listening socket objects?

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


int	main(void)
{
	struct sockaddr_storage client_addr;
	char buffer[BUFFER_SIZE];
	socklen_t addr_size;
	int num_listening_sockets = 2; // getting this info from config file

	// setting up listening sockets
	std::vector<ListeningSocket> listening_sockets = createSockets();

	//listening to incoming requests and setting listening socket to non-blocking (actually does not matter that much as long as I/O is not edge-triggered)
	for (std::vector<ListeningSocket>::iterator it = listening_sockets.begin(); it != listening_sockets.end(); it++)
	{
		setNonblocking(it->getSocketFd());
		if (listen(it->getSocketFd(), SOMAXCONN) < 0)
			return (perror("Failure when listening for requests"), 1);
	}

	// create kqueue object
	int kq_fd = kqueue();
	if (kq_fd == -1)
		return (perror("Failure when creating kqueue object"), 1);
	
	// attach sockets to kqueue
	// define what events we are interested in (in case of the listening socket we are only interested in the EVFILT_READ
	// since it is only used for accepting incoming connections)
	struct kevent listening_event[num_listening_sockets];
	struct addrinfo *listening_sock_ident;
	for (int i = 0; i < num_listening_sockets; i++)
		EV_SET(&listening_event[i], listening_sockets[i].getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, (void *)listening_sock_ident);
	if (kevent(kq_fd, listening_event, num_listening_sockets, NULL, 0, NULL) == -1)
		return (perror("Failure in registering event"), 1);

	// create event loop
	// std::vector<struct kevent> eventSet2;
	// struct kevent eventSet[1];

	// q: Accepting requests in a loop and adding the connections to kqueue immediately VS Not accepting requests in a loop and delaying the adding of the connection to the beginning of the event loop when kevent is called anyway
	std::vector<int> pending_fds;


	while (1)
	{
		// check for new events that are registered in our kqueue (could come from a listening or connection socket).
		// 5th arg specifies the number of new events to handle per each iteration
		// it depends on several kernel-internal factors whether kevent returns one or multiple events for several conncetion requests. That's why ideally one makes acception checks in a loop per each event
		struct kevent event_lst[MAX_EVENTS]; // should the corresponding kevent struct be added to a fd class?
		int new_events;
		// printf("size eventSet: %lu\n", eventSet2.size());
		
		new_events = kevent(kq_fd, NULL, 0, event_lst, MAX_EVENTS, NULL);
		if (new_events == -1)
			return (perror("Failure when checking for new events"), 1);
		
		// go through all the events we have been notified of
		for (int i = 0; new_events > i; i++)
		{
			printf("new event detected; num events: %d\n", new_events);
			printf("data: %li\n", event_lst[i].data); //--> this the size of the listen backlog

			// when client disconnected an EOF is sent. Close fd to rm event from kqueue
			// event_lst[i].ident is basically the file descriptor of the socket that triggered
			if (event_lst[i].flags & EV_EOF)
			{
				printf("client disconnected\n");
				close(event_lst[i].ident); // is this enough to remove from queue?
			}
			// event came from listening socket --> we have to create the connection
			else if (event_lst[i].udata == listening_sock_ident)
			{
				struct addrinfo *test;
				while (1)
				{
					printf("new connection incoming\n");
					// accept basically performs the 3-way TCP handshake
					// here could actually have a loop and try to make as many accepts as possible (--> see eklitzke)
					// probably need to be store an array or so
					int connection_fd = accept(event_lst[i].ident, (struct sockaddr *)&client_addr, &addr_size);
					if (connection_fd == -1)
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
						{
							int size = pending_fds.size();
							struct kevent eventSet2[size];
							for (int j = 0; j < size; j++)
							{
								setNonblocking(pending_fds[j]);
								EV_SET(&eventSet2[j], pending_fds[j], EVFILT_READ, EV_ADD, 0, 0, (void *)test);
							}
							if (kevent(kq_fd, eventSet2, size, NULL, 0, NULL) < 0)
								return (perror("Failure when registering event"), 1);
							pending_fds.clear();
							break;
						}
						else
							return (perror("Failure when trying to establish connection"), 1);
							// close connection_fd?
					}
					pending_fds.push_back(connection_fd);
					// else
					// {
					// 	setNonblocking(connection_fd);
					// 	// attach conncetion to kqueue to be able to receive updates
					// 	// we later also want to add write
					// 	EV_SET(eventSet, connection_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
					// 	// to further improve performance registration is delayed and done in the first kevent call in the while loop
					// 	// but then, how do we know the size of eventSet?
					// 	if (kevent(kq_fd, eventSet, 1, NULL, 0, NULL) < 0)
					// 		return (perror("Failure when registering event"), 1);
					// }
				}
				
			}

			// event came from conncetion socket and had been added with read filter
			// signalling that it is ready for reading --> so, we want to read as much as possible
			else if (event_lst[i].filter == EVFILT_READ)
			{
				char buf[BUFFER_SIZE];
				size_t bytes_read = recv(event_lst[i].ident, buf, sizeof(buf), 0);
				printf("read %zu bytes\n", bytes_read);
			}
		}
	}
	// close all listening sockets (this removes them from kqueue) --> do I need to do something similar with the connection sockets
	for (std::vector<ListeningSocket>::iterator it = listening_sockets.begin(); it != listening_sockets.end(); it++)
	{
		close(it->getSocketFd());
	}
	close(kq_fd);

}



// epoll/kqueue etc. provide an interface to kernel
// 1) creating a KQ object (one per provess/thread)
// 2) attach socket_fd to KQ
// --> since we have several socket_fds, we would need to attach several?
// --> this is usually done once per socket
// 3) wait for incoming events from KQ
// 4) destroy KQ object

// how to test?

