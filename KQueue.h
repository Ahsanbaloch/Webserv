
#ifndef KQUEUE_H
# define KQUEUE_H

#include <sys/event.h>
#include <unistd.h>
#include <vector>
#include "ServerConfig.h"

class KQueue
{
private:
	/* data */
public:
	int	kqueue_fd;
	int listening_sock_ident;
	int connection_sock_ident;
	KQueue(/* args */);
	~KQueue();

	int	attachListeningSockets(ServerConfig Server);
	int attachConnectionSockets(std::vector<int> pending_fds);
	void closeQueue();
};


#endif
