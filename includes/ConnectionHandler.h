#ifndef CONNECTIONHANDLER_H
# define CONNECTIONHANDLER_H

# include <vector>
# include "config/config_pars.hpp"

class ConnectionHandler
{
private:
	int	connection_fd;
	std::vector<t_server_config>	server_config;
public:
	ConnectionHandler(/* args */);
	ConnectionHandler(int, std::vector<t_server_config>);
	~ConnectionHandler();
};

#endif
