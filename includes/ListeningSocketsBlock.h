#ifndef LISTENINGSOCKETSBLOCK_H
# define LISTENINGSOCKETSBLOCK_H

#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <string>
#include "CustomException.h"
#include "ListeningSocket.h"
#include "config/config_pars.hpp"

class ListeningSocketsBlock
{
private:
	
public:
	int								num_listening_sockets; // getting this info from config file
	std::map<int, ListeningSocket>	listening_sockets;

	explicit ListeningSocketsBlock(std::map<std::string, std::vector<t_server_config> >&);
	ListeningSocketsBlock();
	~ListeningSocketsBlock();

	void	createSockets(std::map<std::string, std::vector<t_server_config> >&);
	void	closeSockets();
};


#endif
