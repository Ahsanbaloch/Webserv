#ifndef CONNECTIONHANDLER_H
# define CONNECTIONHANDLER_H

# include <vector>
# include "config/config_pars.hpp"
# include "RequestHandler.h"

class ConnectionHandler
{
private:
	RequestHandler* 				handler;
	std::vector<t_server_config>	server_config;
	int								connection_fd;
	bool							response_ready;
	
	// constructors
	ConnectionHandler();
	ConnectionHandler(const ConnectionHandler&);
	ConnectionHandler&	operator=(const ConnectionHandler&);

public:
	// constructors & destructor
	ConnectionHandler(int, std::vector<t_server_config>);
	~ConnectionHandler();

	// getters
	RequestHandler*	getRequestHandler();
	int				getResponseStatus();

	// setters
	void			initRequestHandler();
	void			setResponseStatus(int);

	// methods
	void			removeRequestHandler();

};

#endif
