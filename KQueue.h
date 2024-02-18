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
	int listening_sock_ident;
	int connection_sock_ident;
	KQueue();
	~KQueue();
};


#endif
