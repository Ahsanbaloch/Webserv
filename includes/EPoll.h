
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
	uint32_t	getListeningSockIdent() const;
	uint32_t	getConnectionSockIdent() const;

	// methods
	void	attachListeningSockets(ListeningSocketsBlock&);
	void	attachConnectionSockets(std::vector<int>);
	void	closeQueue();
	
};


#endif
