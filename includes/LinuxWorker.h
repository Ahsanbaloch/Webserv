#ifndef LINUXWORKER_H
# define LINUXWORKER_H

# include <sys/socket.h>
# include <sys/epoll.h>
# include <cerrno>
# include <iostream>
# include <vector>
# include <map>
# include <algorithm>
# include "CustomException.h"
# include "EPoll.h"
# include "RequestHandler.h"
# include "ConnectionHandler.h"
# include "ListeningSocketsBlock.h"
# include "config/config_pars.hpp"
# include "defines.h"


class LinuxWorker
{
private:
	// vars
	EPoll								Q;
	struct epoll_event					event_lst[MAX_EVENTS];
	std::map<int, ListeningSocket>		listening_sockets;
	std::map<int, ConnectionHandler*>	connected_clients;
	std::vector<int>					pending_fds;
	struct sockaddr						client_addr;
	socklen_t							addr_size;

	// helper methods
	void	addToConnectedClients(ListeningSocket&);
	void	closeConnection(int);
	void	acceptConnections(int);
	void	handleReadEvent(int);
	void	handleWriteEvent(int);
	void	handleCGIResponse(int);

	// constructors
	LinuxWorker();
	LinuxWorker(const LinuxWorker&);
	LinuxWorker& operator=(const LinuxWorker&);

public:
	// constructors & desctructors
	explicit	LinuxWorker(const EPoll&, ListeningSocketsBlock&);
	~LinuxWorker();

	// main method
	void	runEventLoop();

};


#endif
