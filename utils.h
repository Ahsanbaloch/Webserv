
#ifndef UTILS_H
# define UTILS_H

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>
#include <map>
#include <vector>
#include <iostream>
#include <string>

#include "ListeningSocket.h"
#include "KQueue.h"

#define BUFFER_SIZE 1024
#define MAX_EVENTS 128 // how to determine what to set here? --> maybe partly related to SOMAXCONN, but apparently not entirely


void							setNonblocking(int fd);
std::vector<ListeningSocket>	createSockets();
void	handleRequest(struct kevent event_lst_item);
int	addConnectionToKernelQueue(KQueue Queue, std::vector<int> pending_fds, struct addrinfo *connection_sock_ident);
int	runEventLoop(KQueue Queue);


#endif