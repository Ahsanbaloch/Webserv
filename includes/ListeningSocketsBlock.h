#ifndef LISTENINGSOCKETSBLOCK_H
# define LISTENINGSOCKETSBLOCK_H

#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include "CustomException.h"
#include "ListeningSocket.h"

class ListeningSocketsBlock
{
private:
	
public:
	int								num_listening_sockets; // getting this info from config file
	std::vector<ListeningSocket>	listening_sockets;

	explicit ListeningSocketsBlock(std::vector<int>);
	ListeningSocketsBlock();
	~ListeningSocketsBlock();

	void	createSockets(std::vector<int>);
	void	closeSockets();
};


#endif
