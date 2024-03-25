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
	
	// extruded info from request
	std::string							method;
	std::string							path;
	std::string							query;
	std::string							version;
	std::map<std::string, std::string>	header_fields;

	// flags
	bool								header_complete;
	bool								rl_parsing_done;
	bool								headers_parsing_done;
	bool								transfer_encoding_exists;
	bool								content_length_exists;
	bool								host_exists;
	bool								path_encoded;
	bool								query_encoded;
	bool								dot_in_path;
	
	// main methods
	void								parseRequestLine();
	void								parseHeaderFields();
	void								removeDots();
	void								checkFields();

	// helper methods
	void								handleMultipleSlashes();
	void								checkMethod();
	void								decode(std::string&);
	void								checkBodyLength(std::string);
	void								checkHttpVersion();
	std::vector<std::string>			splitPath(std::string, char); // maybe move somewhere else as similar as in ARquest class

	// constructors
	Header();
	Header(const Header&);
	Header& operator=(const Header&);

public:
	// constructors & destructors
	explicit Header(RequestHandler&);
	~Header();
	
	// getters
	std::string							getMethod() const;
	std::string 						getPath() const;
	std::string							getQuery() const;
	std::string							getHttpVersion() const;
	std::map<std::string, std::string>	getHeaderFields() const;
	bool								getHeaderStatus() const;

	// TBD: move to request class???
	std::string							redirected_path;

	// method
	void								parseHeader();

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
