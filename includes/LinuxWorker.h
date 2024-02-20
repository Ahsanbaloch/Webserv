#ifndef LINUXWORKER_H
# define LINUXWORKER_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <cerrno>
#include <iostream>
#include <vector>
#include <algorithm>
#include "CustomException.h"
#include "EPoll.h"
#include "RequestHandler.h"

#define MAX_EVENTS 128 // how to determine what to set here? --> maybe partly related to SOMAXCONN, but apparently not entirely

class LinuxWorker
{
private:
	/* data */
public:
	EPoll				Q;
	RequestHandler		Handler;
	std::vector<int>	pending_fds;
	std::vector<int>	listening_socks_fd;
	struct sockaddr		client_addr;
	socklen_t			addr_size;
	struct epoll_event	event_lst[MAX_EVENTS];

	explicit	LinuxWorker(const EPoll&);
	LinuxWorker();
	~LinuxWorker();

	void	runEventLoop();
};


#endif
