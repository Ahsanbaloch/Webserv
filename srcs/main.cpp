
#ifdef __APPLE__
	#include "KQueue.h"
	#include "DarwinWorker.h"
#else
	#include "EPoll.h"
	#include "LinuxWorker.h"
#endif
#include "ListeningSocketsBlock.h"
#include "config/config_pars.hpp"
#include "defines.h"


int	main(int argc, char **argv)
{
	srand(time(0));

	try
	{
		// Parse configuration file
		config_pars config(argc, argv);
		std::map<std::string, std::vector<t_server_config> > serverConfigsMap = config.getServerConfigsMap();

		// Create listening sockets, bind, set non-blocking, listen
		ListeningSocketsBlock SocketsBlock(serverConfigsMap);

		// create KQueue object
		#ifdef __APPLE__
			KQueue Queue;
			// attach sockets to kqueue
			Queue.attachListeningSockets(SocketsBlock);
			// create Worker object
			DarwinWorker Worker(Queue, SocketsBlock);
		#else
			EPoll Queue;
			Queue.attachListeningSockets(SocketsBlock);
			LinuxWorker Worker(Queue, SocketsBlock);
		#endif

		// run event loop
		Worker.runEventLoop();

		// close all listening sockets (this removes them from kqueue)
		SocketsBlock.closeSockets();

		// close connection sockets? (if any are left?)

		// close queue
		Queue.closeQueue();
	}
	catch(const std::exception& e)
	{
		// close fds?
		std::cerr << e.what() << '\n';
	}
}
