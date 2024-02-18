
#include "utils.h"
#include "ServerConfig.h"
#include "DarwinWorker.h"

int	main(void)
{
	// Create Server object (create listening sockets and bind them)
	ServerConfig Server;

	//listening to incoming requests and setting listening socket to non-blocking (actually does not matter that much as long as I/O is not edge-triggered)
	for (std::vector<ListeningSocket>::iterator it = Server.listening_sockets.begin(); it != Server.listening_sockets.end(); it++)
	{
		setNonblocking(it->getSocketFd());
		if (listen(it->getSocketFd(), SOMAXCONN) < 0)
			return (perror("Failure when listening for requests"), 1);
	}

	// create kqueue object
	DarwinWorker Worker;
	
	// attach sockets to kqueue
	// define what events we are interested in (in case of the listening socket we are only interested in the EVFILT_READ
	// since it is only used for accepting incoming connections)
	struct kevent listening_event[Server.num_listening_sockets];
	// struct addrinfo *listening_sock_ident;
	for (int i = 0; i < Server.num_listening_sockets; i++)
		EV_SET(&listening_event[i], Server.listening_sockets[i].getSocketFd(), EVFILT_READ, EV_ADD, 0, 0, &Worker.listening_sock_ident);
	if (kevent(Worker.kqueue_fd, listening_event, Server.num_listening_sockets, NULL, 0, NULL) == -1)
		return (perror("Failure in registering event"), 1);

	// run event loop
	// Queue.runEventLoop();
	runEventLoop(Worker);

	// close all listening sockets (this removes them from kqueue) --> do I need to do something similar with the connection sockets
	for (std::vector<ListeningSocket>::iterator it = Server.listening_sockets.begin(); it != Server.listening_sockets.end(); it++)
	{
		close(it->getSocketFd());
	}
	close(Worker.kqueue_fd);
}
