
#ifndef LISTENINGSOCKET_HPP
# define LISTENINGSOCKET_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <iostream>

class ListeningSocket
{
private:
	int	socket_fd;
	struct sockaddr_in sock_config;
	ListeningSocket();
public:
	explicit ListeningSocket(int);
	~ListeningSocket();
	void	setSockOptions();
	void	initSockConfig(int, u_int32_t);
	void	bindSock();
	int		getSocketFd() const;
};

#endif

