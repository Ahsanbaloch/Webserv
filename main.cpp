
#include "ServerConfig.h"
#include "KQueue.h"
#include "DarwinWorker.h"

// next:
// - finish clean up -> partiuclarly Server Config / listeningSocket class
// - check exception handling
// - create Makefile
// - add support for write requests

int	main(void)
{
	// Create Server object (create listening sockets and bind them)
	ServerConfig Server;

	//listening to incoming requests and setting listening socket to non-blocking (actually does not matter that much as long as I/O is not edge-triggered)
	Server.listen2();

	// create KQueue object
	KQueue Queue;

	// attach sockets to kqueue
	Queue.attachListeningSockets(Server);

	// create Worker object
	DarwinWorker Worker(Queue);

	// run event loop
	Worker.runEventLoop();

	// close all listening sockets (this removes them from kqueue) --> do I need to do something similar with the connection sockets
	Server.closeSockets();

	// close connection sockets?

	// close queue
	Queue.closeQueue();
}
