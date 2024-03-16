
#ifndef LISTENINGSOCKET_H
# define LISTENINGSOCKET_H

# include <netinet/in.h>
# include <fcntl.h>
# include <iostream>
# include <cstring> // check if needed?
# include <vector>
# include <string>
#include "CustomException.h"
#include "config/config_pars.hpp"

class ListeningSocket
{
private:
	int					socket_fd;
	struct sockaddr_in	sock_config;
	ListeningSocket();
public:
	explicit ListeningSocket(int fd);
	~ListeningSocket();

	int			getSocketFd() const;

	void		setSockOptions();
	void		initSockConfig(std::vector<t_server_config>&);
	void		bindSock();
	void		makeListen();
	static void	setNonblocking(int);
};

#endif

