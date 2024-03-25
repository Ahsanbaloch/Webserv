#ifndef HEADER_H
# define HEADER_H

# include <string>
# include <map>
# include <vector>
# include <sstream>
# include <iostream>
# include "defines.h"
# include "CustomException.h"

class RequestHandler;

class Header
{
private:
	RequestHandler&						handler;
	std::string							method;
	std::string							version;
	std::string							query;
	std::string							path;
	std::map<std::string, std::string>	header_fields;

	// flags
	bool								rl_parsing_done;
	bool								headers_parsing_done;
	bool								transfer_encoding_exists;
	bool								content_length_exists;
	bool								host_exists;
	bool								path_encoded;
	bool								query_encoded;
	
	// helper functions
	void								handleMultipleSlashes();
	void								checkMethod();
	void								decodeRequestLine(std::string&);
	void								checkBodyLength(std::string);
	void								checkHttpVersion();
	std::vector<std::string>			splitPath(std::string, char); // maybe move somewhere else as similar as in ARquest class

public:

	std::string							getMethod() const;
	std::string							getHttpVersion() const;
	std::string							getQuery() const;
	std::string 						getPath() const;
	std::map<std::string, std::string>	getHeaderFields() const;

	// Header(/* args */);
	~Header();
	explicit Header(RequestHandler&);

	
	std::string							redirected_path;
	
	
	int									expect_exists;
	int									body_length; // probably move some like these into a dedicated class

	int									header_complete;
	int									body_beginning;
	bool								dot_in_path;
	
	
	void								parseRequestLine();
	void								parseHeaderFields();
	void								removeDots();
	void								decode();
	void								checkFields();

	// parsing states
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
