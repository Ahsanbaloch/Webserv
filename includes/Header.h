#ifndef HEADER_H
# define HEADER_H

# include <string>
# include <map>
# include <vector>
# include <sstream>
# include <iostream>
# include "CustomException.h"
// # include "RequestHandler.h"

// should be defined somewhere else in extra class
#define LF 10
#define CR 13
#define SP 32

class RequestHandler;

class Header
{
private:
	/* data */
public:
	Header(/* args */);
	~Header();

	std::string							method; // probably needs to be reset after being used
	std::string							path; // probably needs to be reset after being used
	std::string							redirected_path;
	std::string							query; // probably needs to be reset after being used
	std::string							version; // probably needs to be reset after being used
	std::map<std::string, std::string>	header_fields; // rename to header_fields // probably needs to be reset after being used

	int									rl_parsing_done; // probably needs to be reset after being used
	int									headers_parsing_done; // probably needs to be reset after being used
	int									transfer_encoding_exists;
	int									content_length_exists;
	int									host_exists;
	int									expect_exists;
	int									body_length; // probably move some like these into a dedicated class

	int									header_complete;
	int									body_beginning;

	int									path_encoded; // probably needs to be reset after being used
	int									query_encoded;
	int									dot_in_path;
	// int									field_encoded;
	// std::vector<std::string>			fields_encoded;
	int									error;

	// void	parseRequestLine(unsigned char* buf, int* buf_pos, int bytes_read);
	void	parseRequestLine(RequestHandler&); // could the reference also be provided when constructing the object?
	void	parseHeaderFields(RequestHandler&);
	// void	parseHeaderFields(unsigned char* buf, int* buf_pos, int bytes_read);
	// void	checkMethod(unsigned char* buf, int* buf_pos);
	void	checkMethod(RequestHandler&);
	void	checkHttpVersion(RequestHandler&);
	// void	checkHttpVersion(unsigned char* buf, int* buf_pos);
	void	checkBodyLength(std::string);
	void	decode();
	void	decodeRequestLine(std::string&);
	void	checkFields();
	void	handleMultipleSlashes(RequestHandler&);
	void	removeDots();
	// void	decodeHeaderFields();

	std::vector<std::string>	splitPath(std::string, char);

	enum {
		rl_start = 0,
		rl_method,
		rl_first_divider,
		rl_path,
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

};

#endif
