#ifndef HEADER_H
# define HEADER_H

# include <string>
# include <map>
# include <iostream>
# include "CustomException.h"

// should be defined somewhere else in extra class
#define LF 10
#define CR 13
#define SP 32

class Header
{
private:
	/* data */
public:
	Header(/* args */);
	~Header();

	std::string							method; // probably needs to be reset after being used
	std::string							path; // probably needs to be reset after being used
	std::string							query; // probably needs to be reset after being used
	std::string							version; // probably needs to be reset after being used
	std::map<std::string, std::string>	header_fields; // rename to header_fields // probably needs to be reset after being used

	int									rl_parsing_done; // probably needs to be reset after being used
	int									headers_parsing_done; // probably needs to be reset after being used
	int									transfer_encoding_exists;
	int									content_length_exists;
	int									host_exists;
	int									expect_exists;
	int									body_length;

	int									header_complete;
	int									body_beginning;

	int									path_encoded; // probably needs to be reset after being used
	int									error;

	void	parseRequestLine(unsigned char* buf, int* buf_pos, int bytes_read);
	void	parseHeaderFields(unsigned char* buf, int* buf_pos, int bytes_read);
	void	checkMethod(unsigned char* buf, int* buf_pos);
	void	checkHttpVersion(unsigned char* buf, int* buf_pos);
	void	checkBodyLength(std::string);

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

};

#endif
