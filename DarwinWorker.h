#ifndef DARWINWORKER_H
# define DARWINWORKER_H

#include <sys/socket.h>
#include <sys/event.h>
#include <sys/types.h>
#include <iostream>
#include <vector>

class DarwinWorker
{
private:
	/* data */
public:
	int kqueue_fd;
	int listening_sock_ident;
	int connection_sock_ident;
	DarwinWorker(/* args */);
	~DarwinWorker();
};


#endif
