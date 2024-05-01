#ifndef LISTENINGSOCKETSBLOCK_H
# define LISTENINGSOCKETSBLOCK_H

# include <fcntl.h>
# include <unistd.h>
# include <vector>
# include <map>
# include <string>
# include "CustomException.h"
# include "ListeningSocket.h"
# include "config/config_pars.hpp"

class ListeningSocketsBlock
{
private:
	// vars
	int								num_listening_sockets;
	std::map<int, ListeningSocket>	listening_sockets;

	// constructors
	ListeningSocketsBlock();
	ListeningSocketsBlock(const ListeningSocketsBlock&);
	ListeningSocketsBlock& operator=(const ListeningSocketsBlock&);
	
public:
	// constructors & destructors
	explicit ListeningSocketsBlock(std::map<std::string, std::vector<t_server_config> >&);
	~ListeningSocketsBlock();

	// getters
	int								getNumListeningSockets() const;
	std::map<int, ListeningSocket>	getListeningSockets() const;

	// methods
	void	createSockets(std::map<std::string, std::vector<t_server_config> >&);
	void	closeSockets();
};


#endif
