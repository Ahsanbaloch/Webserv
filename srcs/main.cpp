
#include "../includes/ListeningSocketsBlock.h"
#include "../includes/KQueue.h"
#include "../includes/DarwinWorker.h"

// next:
// - create Makefile
// - add support for write requests

int	main(void)
{
	// for testing multiple ports --> info incl. ip comes from config file
	std::vector<int> ports_test;
	ports_test.push_back(4241);
	ports_test.push_back(8081);

	try
	{
		// Create Server object (create listening sockets, bind, set non-blocking, listen)
		ListeningSocketsBlock SocketsBlock(ports_test);

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
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		// close fds?
	}
	
}
