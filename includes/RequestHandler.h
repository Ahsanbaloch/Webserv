#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

#include <sys/socket.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstdio>
#include "CustomException.h"
#include "RequestHeader.h"
#include "ARequestBody.h"
#include "MULTIPARTBody.h"
#include "PLAINBody.h"
#include "URLENCODEDBody.h"
#include "AResponse.h"
#include "GETResponse.h"
#include "DELETEResponse.h"
#include "ERRORResponse.h"
#include "POSTResponse.h"
#include "config/config_pars.hpp"
#include "defines.h"


class RequestHandler
{
private:
	RequestHeader					request_header;
	ARequestBody*					request_body;
	AResponse*						response;

	// vars
	std::vector<t_server_config>	server_config;
	int								status;
	int								selected_location;
	int								selected_server;
	int								connection_fd;
	int								bytes_read;
	int								request_length;

	// flags
	bool							response_ready;
	
	// constructors
	RequestHandler(const RequestHandler&);

public:
	// constructors & destructors
	RequestHandler(int, std::vector<t_server_config>); // get ServerConfig as a reference? // might be able to remove int connection_fd as this is now part of the connection handler
	RequestHandler();
	~RequestHandler();
	RequestHandler& operator=(const RequestHandler&);

	// getters
	std::vector<t_server_config>	getServerConfig() const;
	s_location_config				getLocationConfig() const;
	int								getSelectedLocation() const; // only for testing purposes
	int								getStatus() const;
	bool							getResponseStatus() const;
	int								getBytesRead() const;
	int								getRequestLength() const;
	const RequestHeader&			getHeaderInfo();

	// setters
	void							setStatus(int);
	
	// buffer TBD
	unsigned char					buf[BUFFER_SIZE + 1];
	int								buf_pos;
	
	// methods
	void							processRequest();
	void							sendResponse();
	void							findServerBlock();
	void							findLocationBlock();
	int								calcMatches(std::vector<std::string>&, std::vector<std::string>&); // make private?
	std::vector<std::string>		splitPath(std::string input, char delim);
	AResponse*						prepareResponse();
	ARequestBody*					checkContentType();

	// make private?
	enum {
		unknown = 0,
		multipart_form,
		text_plain,
		urlencoded
	} content_type;
};

#endif
