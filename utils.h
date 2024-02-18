
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
#include "DarwinWorker.h"


#define BUFFER_SIZE 1024

void							setNonblocking(int fd);
std::vector<ListeningSocket>	createSockets();
void	handleRequest(struct kevent event_lst_item);
// int	addConnectionToKernelQueue(DarwinWorker Worker, std::vector<int> pending_fds);
// int	runEventLoop(DarwinWorker Worker);


#endif