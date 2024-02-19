#ifndef LISTENINGSOCKETSBLOCK_H
# define LISTENINGSOCKETSBLOCK_H

#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include "ListeningSocket.h"

class ListeningSocketsBlock
{
private:
	/* data */
public:
	int num_listening_sockets; // getting this info from config file
	std::vector<ListeningSocket> listening_sockets;
	ListeningSocketsBlock();
	~ListeningSocketsBlock();
	std::vector<ListeningSocket>	createSockets();
	void closeSockets();
};




#endif
