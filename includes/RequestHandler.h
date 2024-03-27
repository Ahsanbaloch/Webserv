#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

#include <sys/socket.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstdio>
#include "CustomException.h"
#include "Header.h"
#include "ARequest.h"
#include "GETRequest.h"
#include "DELETERequest.h"
#include "config/config_pars.hpp"
#include "defines.h"


class RequestHandler
{
private:
	Header							header;

	std::vector<t_server_config>	server_config;
	int								status;
	int								selected_location;
	int								selected_server;
	int								connection_fd;
	int								bytes_read;

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
	const Header&					getHeaderInfo();

	// setters
	void							setStatus(int);

	ARequest*						request;
	Response*						response;
	
	
	// tbd
	int								body_parsing_done;
	int								chunk_length;
	int								request_length;
	int								body_read;
	int								body_beginning;
	int								body_length; // set inside header class
	std::stringstream				raw_buf;
	std::stringstream				body;
	unsigned char					buf[BUFFER_SIZE]; // use std::vector<unsigned char> buf(BUFFER_SIZE) or uint8_t instead? // don't use the string type for your buffer because for upload and binary file you can have some \0 in the middle of the content). + CRLF interpretation --> request smuggling
	int								buf_pos;
	void							parseEncodedBody();
	void							parseBody();
	
	// methods
	void							processRequest();
	void							sendResponse();
	void							findServerBlock();
	void							findLocationBlock();
	int								calcMatches(std::vector<std::string>&, std::vector<std::string>&); // make private?
	std::vector<std::string>		splitPath(std::string input, char delim);
	ARequest*						newRequest();

	enum {
		body_start = 0,
		chunk_size,
		chunk_size_cr,
		chunk_extension,
		chunk_data,
		chunk_data_cr,
		chunk_trailer,
		body_end
	} te_state;

};

#endif
