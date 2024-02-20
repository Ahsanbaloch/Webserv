#ifndef DARWINWORKER_H
# define DARWINWORKER_H

#include <sys/socket.h>
#include <sys/event.h>
#include <iostream>
#include <vector>
#include "CustomException.h"
#include "KQueue.h"
#include "RequestHandler.h"

#define MAX_EVENTS 128 // how to determine what to set here? --> maybe partly related to SOMAXCONN, but apparently not entirely

class DarwinWorker
{
private:
	/* data */
public:
	KQueue				Q;
	RequestHandler		Handler;
	std::vector<int>	pending_fds;
	struct sockaddr		client_addr;
	socklen_t			addr_size;
	struct kevent		event_lst[MAX_EVENTS];

	explicit	DarwinWorker(const KQueue&);
	DarwinWorker();
	~DarwinWorker();

	void	runEventLoop();
};


#endif
