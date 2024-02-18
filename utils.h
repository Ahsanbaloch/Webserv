
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



void							setNonblocking(int fd);
std::vector<ListeningSocket>	createSockets();



#endif