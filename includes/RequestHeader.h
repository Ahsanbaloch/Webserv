#ifndef REQUESTHEADER_H
# define REQUESTHEADER_H

# include <string>
# include <map>
# include <vector>
# include <sstream>
# include <iostream>
# include "defines.h"
# include "CustomException.h"
# include "utils.h"
# include "utils.tpp"

class RequestHandler;

class RequestHeader
{
private:
	RequestHandler&						handler;
	
	// extruded info from request
	std::string							method;
	std::string							path;
	std::string							query;
	std::string							version;
	std::map<std::string, std::string>	header_fields;
	std::string							filename;
	std::string							file_ext;
	int									body_beginning;
	int									body_length;

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
	bool								body_expected;
	bool								expect_exists;
	
	// main methods
	void								removeDots();
	void								checkFields();

	// helper methods
	void								handleMultipleSlashes();
	void								checkMethod();
	void								checkBodyLength(std::string);
	void								checkHttpVersion();
	void								decode(std::string&);

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

	// constructors
	RequestHeader();

public:
	// constructors & destructors
	explicit RequestHeader(RequestHandler&);
	RequestHeader(const RequestHeader&);
	RequestHeader& operator=(const RequestHeader&);
	~RequestHeader();
	
	// getters
	std::string							getMethod() const;
	std::string 						getPath() const;
	std::string							getQuery() const;
	std::string							getHttpVersion() const;
	std::map<std::string, std::string>	getHeaderFields() const;
	std::string							getFilename() const;
	std::string							getFileExtension() const;
	bool								getRequestLineStatus() const;
	bool								getHeaderProcessingStatus() const;
	bool								getBodyExpectanceStatus() const;
	bool								getHeaderExpectedStatus() const;
	bool								getTEStatus() const;
	int									getBodyBeginning() const;
	int									getBodyLength() const;

	// methods
	void								parseRequestLine();
	void								parseHeaderFields();
	void								identifyFileName();
	void								checkHeader();
	void								makeInternalRedirect(std::string);

};

#endif
