
#ifndef LISTENINGSOCKET_H
# define LISTENINGSOCKET_H

# include <netinet/in.h>
# include <fcntl.h>
# include <iostream>
# include <cstring> // check if needed?
# include <vector>
# include <string>
# include <unistd.h>
# include "CustomException.h"
# include "config/config_pars.hpp"

class ListeningSocket
{
private:
	// vars
	std::vector<t_server_config>	server_config;
	int								socket_fd;
	struct sockaddr_in				sock_config;
	
	// constructors
	ListeningSocket();
	ListeningSocket& operator=(const ListeningSocket&);

public:
	// constructor & destructors
	ListeningSocket(int, std::vector<t_server_config>);
	ListeningSocket(const ListeningSocket&);
	~ListeningSocket();

	// getters
	int								getSocketFd() const;
	std::vector<t_server_config>	getServerConfig() const;

	// methods
	void							setSockOptions();
	void							initSockConfig();
	void							bindSock();
	void							makeListen();
	static void						setNonblocking(int);

};

#endif

