#ifndef CONNECTIONHANDLER_H
# define CONNECTIONHANDLER_H

# include <vector>
# include "config/config_pars.hpp"
# include "RequestHandler.h"

class ConnectionHandler
{
private:
	//vars
	RequestHandler* 				handler;
	std::vector<t_server_config>	server_config;
	int								connection_fd;
	int								kernel_q_fd;

	//flags
	bool							response_ready;
	
	// constructors
	ConnectionHandler();
	ConnectionHandler(const ConnectionHandler&);
	ConnectionHandler&	operator=(const ConnectionHandler&);

public:
	// constructors & destructor
	ConnectionHandler(int, std::vector<t_server_config>, int);
	~ConnectionHandler();

	// getters
	RequestHandler*					getRequestHandler();
	int								getResponseStatus() const;

	// setters
	void							initRequestHandler();
	void							setResponseStatus(bool);

	// methods
	void							removeRequestHandler();

};

#endif
