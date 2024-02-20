
#ifndef EPOLL_H
# define EPOLL_H

#include <sys/epoll.h> // may have to change something in the makefile?
#include <unistd.h>
#include <vector>
#include "CustomException.h"
#include "ListeningSocketsBlock.h"

class EPoll
{
private:
	/* data */
public:
	ListeningSocketsBlock SocketsBlock;
	int	epoll_fd;
	// int	listening_sock_ident;
	// int	connection_sock_ident;

	explicit EPoll(const ListeningSocketsBlock&);
	EPoll();
	~EPoll();

	void	attachListeningSockets();
	void	attachConnectionSockets(std::vector<int>);
	void	closeQueue();
};


#endif
