#ifndef CONNECTIONHANDLER_H
# define CONNECTIONHANDLER_H

# include <vector>
# include "config/config_pars.hpp"
# include "RequestHandler.h"
# include "KQueue.h"

class ConnectionHandler
{
private:
	//vars
	RequestHandler* 				handler;
	const KQueue&					Q;
	std::vector<t_server_config>	server_config;
	int								connection_fd;

	//flags
	bool							response_ready;
	
	// constructors
	ConnectionHandler();
	ConnectionHandler(const ConnectionHandler&);
	ConnectionHandler&	operator=(const ConnectionHandler&);

public:
	// constructors & destructor
	ConnectionHandler(int, std::vector<t_server_config>, const KQueue&);
	~ConnectionHandler();

	// getters
	RequestHandler*					getRequestHandler();
	int								getResponseStatus();

	// setters
	void							initRequestHandler();
	void							setResponseStatus(bool);

	// methods
	void							removeRequestHandler();

};

#endif
