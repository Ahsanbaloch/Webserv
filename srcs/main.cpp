
#ifdef __APPLE__
	#include "KQueue.h"
	#include "DarwinWorker.h"
#else
	#include "EPoll.h"
	#include "LinuxWorker.h"
#endif
#include "ListeningSocketsBlock.h"
#include "config/config_pars.hpp"


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

		// Set up server
		#ifdef __APPLE__
			KQueue Queue;
			Queue.attachListeningSockets(SocketsBlock);
			DarwinWorker Worker(Queue, SocketsBlock);
		#else
			EPoll Queue;
			Queue.attachListeningSockets(SocketsBlock);
			LinuxWorker Worker(Queue, SocketsBlock);
		#endif

		// run server
		Worker.runEventLoop();

		// close all listening sockets
		SocketsBlock.closeSockets();

		// close queue
		Queue.closeQueue();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
