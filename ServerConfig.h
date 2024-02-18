#ifndef SERVERCONFIG_H
# define SERVERCONFIG_H

#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include "ListeningSocket.h"

class ServerConfig
{
private:
	/* data */
public:
	int num_listening_sockets; // getting this info from config file
	std::vector<ListeningSocket> listening_sockets;
	ServerConfig();
	~ServerConfig();
	std::vector<ListeningSocket>	createSockets();
	static void						setNonblocking(int fd);
	void	listen2();
	void closeSockets();
};




#endif
