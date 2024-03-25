#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

#include <sys/socket.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstdio> // remove later
#include "CustomException.h"
#include "Header.h"
#include "ARequest.h"
#include "config/config_pars.hpp"

#define BUFFER_SIZE 8192 // this basically presents the max header size (incl. the request line)
// #define LF 10
// #define CR 13
// #define SP 32

class RequestHandler
{
private:
	std::vector<t_server_config>	server_config;
public:

	std::vector<t_server_config>	getServerConfig() const;

	RequestHandler(int, std::vector<t_server_config>);
	RequestHandler(/* args */);
	~RequestHandler();

	ARequest*			request;
	Response*			response;
	Header				header;
	

	int					selected_location; // should probably be in server_config struct
	int					selected_server;
	
	int					connection_fd;
	
	int					body_parsing_done;
	int					chunk_length;
	int					request_length;
	int					status;
	int					response_ready;
	int					body_expected;
	int					body_read;
	
	std::string			file_path;
	std::string			file_type;
	int					url_relocation;

	int					autoindex;

	std::stringstream	raw_buf;
	std::stringstream	body;
	unsigned char		buf[BUFFER_SIZE]; // use std::vector<unsigned char> buf(BUFFER_SIZE) or uint8_t instead? // don't use the string type for your buffer because for upload and binary file you can have some \0 in the middle of the content). + CRLF interpretation --> request smuggling
	int					buf_pos;
	int					bytes_read;

	void	processRequest();
	void	sendResponse();
	void	parseEncodedBody();
	void	parseBody();

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
