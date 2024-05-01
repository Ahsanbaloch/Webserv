
#ifndef EPOLL_H
# define EPOLL_H

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include "CustomException.h"
#include "ListeningSocketsBlock.h"

class EPoll
{
private:
	// vars
	int	epoll_fd;
	int	listening_sock_ident;
	int	connection_sock_ident;

public:
	//constructors & destructors
	EPoll(const EPoll&);
	EPoll& operator=(const EPoll&);
	EPoll();
	~EPoll();

	// getters
	int			getEPOLLFD();
	int			getListeningSockIdent() const;
	int			getConnectionSockIdent() const;

	// methods
	void	attachListeningSockets(ListeningSocketsBlock&);
	void	attachConnectionSockets(std::vector<int>);
	void	closeQueue();

	struct e_data {
		int	fd;
		int socket_ident;
	};
	
};


#endif
