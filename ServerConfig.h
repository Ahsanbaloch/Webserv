#ifndef SERVERCONFIG_H
# define SERVERCONFIG_H

#include <vector>
#include "utils.h"

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
