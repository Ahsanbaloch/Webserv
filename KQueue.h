#ifndef KQUEUE_H
# define KQUEUE_H

#include <sys/socket.h>
#include <sys/event.h>
#include <sys/types.h>
#include <iostream>
#include <vector>

class KQueue
{
private:
public:
	int kqueue_fd;
	struct addrinfo *listening_sock_ident;
	struct addrinfo *connecting_sock_ident;
	KQueue();
	~KQueue();
};


#endif
