
#include "ListeningSocketsBlock.h"
#include "KQueue.h"
#include "DarwinWorker.h"

// next:
// - finish clean up -> partiuclarly Server Config / listeningSocket class
// - check exception handling
// - create Makefile
// - add support for write requests

int	main(void)
{
	// Create Server object (create listening sockets, bind, set non-blocking, listen)
	ListeningSocketsBlock SocketsBlock;

	// create KQueue object
	KQueue Queue;

	// attach sockets to kqueue
	Queue.attachListeningSockets(SocketsBlock);

	// create Worker object
	DarwinWorker Worker(Queue);

	// run event loop
	Worker.runEventLoop();

	// close all listening sockets (this removes them from kqueue) --> do I need to do something similar with the connection sockets
	SocketsBlock.closeSockets();

	// close connection sockets?

	// close queue
	Queue.closeQueue();
}
