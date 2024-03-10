#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

#include <sys/socket.h>
#include <string>
#include <map>
#include <sstream>
#include <cstdio> // remove later
#include "CustomException.h"
#include "Header.h"

#define BUFFER_SIZE 8192 // this basically presents the max header size (incl. the request line)
// #define LF 10
// #define CR 13
// #define SP 32

class RequestHandler
{
private:
	/* data */
public:
	explicit RequestHandler(int);
	RequestHandler(/* args */);
	~RequestHandler();

	int										connection_fd;
	Header									header;
	// std::string							method; // probably needs to be reset after being used
	// std::string							path; // probably needs to be reset after being used
	// std::string							query; // probably needs to be reset after being used
	// std::string							version; // probably needs to be reset after being used
	// std::map<std::string, std::string>	headers; // probably needs to be reset after being used
	// int									error; // probably needs to be reset after being used

	// int									rl_parsing_done;
	// int									headers_parsing_done;
	int									body_parsing_done;
	// int									transfer_encoding_exists;
	// int									content_length_exists;
	// int									host_exists;
	// int									expect_exists;
	// int									body_length;
	int									chunk_length;
	// int									header_complete;
	int									request_length;
	// int									body_beginning; // // this is the last ch of the empty line at the end of the headers. Next ch is the first of the body
	int									error;

	std::stringstream					raw_buf;
	std::stringstream					body;
	unsigned char						buf[BUFFER_SIZE]; // use std::vector<unsigned char> buf(BUFFER_SIZE) or uint8_t instead? // don't use the string type for your buffer because for upload and binary file you can have some \0 in the middle of the content). + CRLF interpretation --> request smuggling
	int									buf_pos;
	int									bytes_read;

	void	handleRequest();
	// void	parseRequestLine();
	// void	parseHeaders();
	void	parseEncodedBody();
	void	parseBody();
	// void	checkMethod();
	// void	checkHttpVersion();
	// void	checkBodyLength(std::string);

	// int	path_encoded; // probably needs to be reset after being used

	// enum {
	// 	rl_start = 0,
	// 	rl_method,
	// 	rl_first_divider,
	// 	rl_path,
	// 	rl_percent_encoded,
	// 	rl_query,
	// 	rl_http,
	// 	rl_cr,
	// 	rl_done
	// } rl_state;

	// enum {
	// 	he_start = 0,
	// 	he_name,
	// 	he_ws_before_value,
	// 	he_value,
	// 	he_ws_after_value,
	// 	he_cr,
	// 	he_done,
	// 	hes_end
	// } headers_state;

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
