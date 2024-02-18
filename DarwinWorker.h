#ifndef DARWINWORKER_H
# define DARWINWORKER_H

#include <sys/socket.h>
#include <sys/event.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include "utils.h"
#include "RequestHandler.h"

#define MAX_EVENTS 128 // how to determine what to set here? --> maybe partly related to SOMAXCONN, but apparently not entirely

class DarwinWorker
{
private:
	/* data */
public:
	KQueue Q;
	RequestHandler RequestHandler;
	std::vector<int> pending_fds;
	struct kevent event_lst[MAX_EVENTS];
	explicit DarwinWorker(KQueue);
	DarwinWorker(/* args */);
	~DarwinWorker();
	int	runEventLoop();
};


#endif
