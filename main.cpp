
#include "utils.h"
#include "ServerConfig.h"
#include "DarwinWorker.h"
#include "KQueue.h"

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

	// create KQueue object
	KQueue Queue;

	// attach sockets to kqueue
	Queue.attachListeningSockets(Server);

	// create Worker object
	DarwinWorker Worker(Queue);

	// run event loop
	Worker.runEventLoop();

	// close all listening sockets (this removes them from kqueue) --> do I need to do something similar with the connection sockets
	for (std::vector<ListeningSocket>::iterator it = Server.listening_sockets.begin(); it != Server.listening_sockets.end(); it++)
	{
		close(it->getSocketFd());
	}
	close(Queue.kqueue_fd);
}
