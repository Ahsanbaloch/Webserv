#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

# include <sys/socket.h>
# include <string>
# include <map>
# include <vector>
# include <sstream>
# include <cstdio>
# include "CustomException.h"
# include "RequestHeader.h"
# include "AUploadModule.h"
# include "UploadMultipart.h"
# include "UploadPlain.h"
# include "UploadUrlencoded.h"
# include "AResponse.h"
# include "GETResponse.h"
# include "DELETEResponse.h"
# include "ERRORResponse.h"
# include "CgiResponse.hpp"
# include "POSTResponse.h"
# include "REDIRECTResponse.h"
# include "BodyExtractor.h"
# include "config/config_pars.hpp"
# include "defines.h"
# include "utils.tpp"


class RequestHandler
{
private:
	RequestHeader					request_header;
	AUploadModule*					uploader;
	AResponse*						response;
	BodyExtractor*					body_extractor;

	// vars
	std::vector<t_server_config>	server_config;
	std::string						new_file_path; // add to header?
	std::string						int_redir_referer_path;
	int								status;
	int								selected_location;
	int								selected_server;
	int								connection_fd;
	int								bytes_read;
	int								request_length;
	int								num_response_chunks_sent;

	int								chunk_length;
	int								total_chunk_size;
	std::ofstream					temp_unchunked;
	bool							trailer_exists;
	bool							body_unchunked;
	std::string						temp_filename_unchunked;

	// flags
	bool							response_ready;
	bool							internal_redirect;
	bool							cgi_post_int_redirect;
	
	// states
	enum {
		body_start = 0,
		chunk_size,
		chunk_size_cr,
		chunk_extension,
		chunk_data,
		chunk_data_cr,
		chunk_trailer,
		chunk_trailer_cr,
		body_end_cr,
		body_end
	} te_state;

	void			unchunkBody();
	void			storeChunkedData();

	// constructors
	RequestHandler(const RequestHandler&);

public:
	// constructors & destructors
	RequestHandler();
	RequestHandler& operator=(const RequestHandler&);
	RequestHandler(int, std::vector<t_server_config>); // get ServerConfig as a reference? // might be able to remove int connection_fd as this is now part of the connection handler
	~RequestHandler();

	// getters
	std::vector<t_server_config>	getServerConfig() const;
	s_location_config				getLocationConfig() const;
	AUploadModule*					getUploader() const;
	AResponse*						getResponseObj() const;
	int								getSelectedLocation() const; // only for testing purposes
	t_server_config					getSelectedServer() const; /// should probably return t_server_config
	int								getStatus() const;
	bool							getResponseStatus() const;
	int								getBytesRead() const;
	int								getRequestLength() const;
	const RequestHeader&			getHeaderInfo();
	std::string						getUnchunkedDataFile() const;
	int								getTotalChunkSize() const;
	bool							getUnchunkingStatus() const;
	std::string						getTempBodyFilepath() const;
	std::string						getIntRedirRefPath() const;
	bool							getIntRedirStatus() const;
	std::string						getNewFilePath() const;
	int								getNumResponseChunks() const;

	// setters
	void							setStatus(int);
	
	// buffer TBD
	unsigned char					buf[BUFFER_SIZE + 1];
	int								buf_pos;
	
	// methods
	void							determineLocationBlock();
	void							processRequest();
	void							sendResponse();
	void							findServerBlock();
	void							findLocationBlock();
	void							checkAllowedMethods();
	void							checkInternalRedirect();
	void							checkMaxBodySize();
	int								calcMatches(std::vector<std::string>&, std::vector<std::string>&); // make private?
	std::vector<std::string>		splitPath(std::string input, char delim);
	AResponse*						prepareResponse();
	AUploadModule*					checkContentType();

	// make private?
	enum {
		unknown = 0,
		multipart_form,
		text_plain,
		urlencoded
	} content_type;
};

#endif
