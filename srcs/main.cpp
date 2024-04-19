
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

int g_num_temp_files = 0;
int g_num_temp_unchunked_files = 0;
int g_num_temp_raw_body = 0;


// next:
// 

int	main(int argc, char **argv)
{
	// for testing multiple ports --> info incl. ip comes from config file
	// std::vector<int> ports_test;
	// ports_test.push_back(4141);
	// ports_test.push_back(8181);

	try
	{
		// Parse configuration file
		config_pars config(argc, argv);
		std::map<std::string, std::vector<t_server_config> > serverConfigsMap = config.getServerConfigsMap();

		// for testing
		// for (std::map<std::string, std::vector<t_server_config> >::iterator it = serverConfigsMap.begin(); it != serverConfigsMap.end(); it++)
		// {
		// 	for (std::vector<t_server_config>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		// 	{
		// 		std::cout << "port and server name: " << it2->port << " " << it2->serverName << std::endl;
		// 		for (std::vector<t_location_config>::iterator it3 = it2->locations.begin(); it3 != it2->locations.end(); it3++)
		// 		{
		// 			/* std::cout << "error status : " << it3->errorPage.error_page_status << std::endl;
		// 			std::cout << "error page : "  << it3->errorPage.html_page << std::endl; */
		// 			std::cout << "upload: " << it3->uploadDir << std::endl;
		// 		}
		// 	}
		// }

		// Create Server object (create listening sockets, bind, set non-blocking, listen)
		ListeningSocketsBlock SocketsBlock(serverConfigsMap); // here goes the config vector; objects will be added to correct listeningSocket (alt: create and return map<socket_fd, ConfigData obj>)

		// create KQueue object
		#ifdef __APPLE__
			KQueue Queue;
			// attach sockets to kqueue
			Queue.attachListeningSockets(SocketsBlock);
			// create Worker object
			DarwinWorker Worker(Queue, SocketsBlock); // also add the SocketsBlock which contains the configData (alt: map<listening_socketfd, config obj>)
		#else
			EPoll Queue(SocketsBlock);
			Queue.attachListeningSockets();
			LinuxWorker Worker(Queue);
		#endif

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
	}
	// close fds?
	
}
