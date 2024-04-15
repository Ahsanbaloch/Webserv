#ifndef DARWINWORKER_H
# define DARWINWORKER_H

#include <sys/socket.h>
#include <sys/event.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include "CustomException.h"
#include "KQueue.h"
#include "RequestHandler.h"
#include "ConnectionHandler.h" // test
#include "ListeningSocketsBlock.h"
#include "config/config_pars.hpp"


#define MAX_EVENTS 128 // how to determine what to set here? --> maybe partly related to SOMAXCONN, but apparently not entirely

class DarwinWorker
{
private:
	/* data */
public:
	KQueue							Q;
	std::map<int, ListeningSocket>	listening_sockets;
	std::map<int, ConnectionHandler*>	ConnectedClients;
	std::vector<int>				pending_fds;
	struct sockaddr					client_addr;
	socklen_t						addr_size;
	struct kevent					event_lst[MAX_EVENTS];

	explicit	DarwinWorker(const KQueue&, ListeningSocketsBlock&);
	DarwinWorker();
	~DarwinWorker();

	void	runEventLoop();
	void	addToConnectedClients(ListeningSocket&);
};


#endif
