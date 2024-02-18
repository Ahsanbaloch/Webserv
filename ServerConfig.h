#ifndef SERVERCONFIG_H
# define SERVERCONFIG_H

#include <vector>
#include "ListeningSocket.h"

std::vector<ListeningSocket>	createSockets();

class ServerConfig
{
private:
	/* data */
public:
	int num_listening_sockets; // getting this info from config file
	std::vector<ListeningSocket> listening_sockets;
	ServerConfig();
	~ServerConfig();
};




#endif
