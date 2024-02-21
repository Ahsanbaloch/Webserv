
#ifndef LISTENINGSOCKET_H
# define LISTENINGSOCKET_H

# include <netinet/in.h>
# include <fcntl.h>
# include <iostream>
# include <cstring>
#include "CustomException.h"

class ListeningSocket
{
private:
	int					socket_fd;
	struct sockaddr_in	sock_config;
	ListeningSocket();
public:
	explicit ListeningSocket(int);
	~ListeningSocket();

	int			getSocketFd() const;

	void		setSockOptions();
	void		initSockConfig(int, u_int32_t);
	void		bindSock();
	void		makeListen();
	static void	setNonblocking(int);
};

#endif

