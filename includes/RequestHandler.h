
#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

#include <sys/socket.h>
#include "CustomException.h"
#include <string>
#include <map>
#include <fstream>
#include <cstdio> // remove later

#define BUFFER_SIZE 1024
#define LF 10
#define CR 13
#define SP 32

class RequestHandler
{
private:
	/* data */
public:
	RequestHandler(/* args */);
	~RequestHandler();

	std::string							method; // probably needs to be reset after being used
	std::string							path; // probably needs to be reset after being used
	std::string							query; // probably needs to be reset after being used
	std::string							version; // probably needs to be reset after being used
	std::map<std::string, std::string>	headers; // probably needs to be reset after being used
	std::fstream						body;
	int									error; // probably needs to be reset after being used
	char								buf[BUFFER_SIZE]; // use std::vector<char> buf(BUFFER_SIZE); instead?
	int									buf_pos;
	int									bytes_read;
	int									rl_parsing_done; // probably needs to be reset after being used
	int									headers_parsing_done;
	int									transfer_encoding_exists;
	int									content_length_exists;
	int									body_length;

	void	handleRequest(int);
	void	parseRequestLine();
	void	parseHeaders();
	void	parseEncodedBody();
	void	parseContentBody();
	void	parseBody();
	void	checkMethod();
	void	checkHttpVersion();
	void	checkBodyLength(std::string);

	int	path_encoded; // probably needs to be reset after being used

	enum {
		rl_start = 0,
		rl_method,
		rl_first_divider,
		rl_path,
		rl_percent_encoded,
		rl_query,
		rl_http,
		rl_cr,
		rl_done
	} rl_state;

	enum {
		he_start = 0,
		he_name,
		he_ws_before_value,
		he_value,
		he_ws_after_value,
		he_cr,
		he_done,
		hes_end
	} headers_state;

	enum {

	} te_state;

};

#endif
