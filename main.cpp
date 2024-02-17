
#include "utils.h"



int	main(void)
{
	char buffer[BUFFER_SIZE];
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

	// run event loop
	runEventLoop(kq_fd, listening_sock_ident);

	// close all listening sockets (this removes them from kqueue) --> do I need to do something similar with the connection sockets
	for (std::vector<ListeningSocket>::iterator it = listening_sockets.begin(); it != listening_sockets.end(); it++)
	{
		close(it->getSocketFd());
	}
	close(kq_fd);
}
