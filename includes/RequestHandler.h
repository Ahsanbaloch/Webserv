#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

# include <sys/socket.h>
# include <string>
# include <map>
# include <vector>
# include <sstream>
# include <cstdio>
# include <algorithm>
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
# include "CGIHandler.h"
# include "CGIResponse.h"
# include "POSTResponse.h"
# include "REDIRECTResponse.h"
# include "BodyExtractor.h"
# include "ChunkDecoder.h"
# include "KQueue.h"
# include "config/config_pars.hpp"
# include "defines.h"
# include "utils.h"
# include "utils.tpp"


class RequestHandler
{
private:
	// objects
	RequestHeader					request_header;
	ChunkDecoder*					chunk_decoder;
	CGIHandler*						cgi_handler;
	AUploadModule*					uploader;
	AResponse*						response;
	BodyExtractor*					body_extractor;
	const KQueue&					Q; // only relevant for MacOS // may just need fd?

	// vars
	std::vector<t_server_config>	server_config;
	std::string						int_redir_referer_path;
	int								status;
	int								selected_location;
	int								selected_server;
	int								connection_fd;
	int								bytes_read;
	int								request_length;
	int								num_response_chunks_sent;

	// flags
	bool							response_ready;
	bool							internal_redirect; // needed?
	bool							all_chunks_sent;
	bool							cgi_detected;
	bool							header_check_done;
	
	// helper methods
	void							receiveRequest();
	void							processHeader();
	void							checkHeader();
	void							processBody();
	void							addCGIToQueue();
	void							makeErrorResponse();

	// constructors
	RequestHandler(const RequestHandler&);

public:
	// constructors & destructors
	RequestHandler();
	RequestHandler& operator=(const RequestHandler&);
	RequestHandler(int, std::vector<t_server_config>, const KQueue&); // get ServerConfig as a reference? // might be able to remove int connection_fd as this is now part of the connection handler
	~RequestHandler();

	// getters
	std::vector<t_server_config>	getServerConfig() const;
	s_location_config				getLocationConfig() const;
	AUploadModule*					getUploader() const;
	ChunkDecoder*					getChunkDecoder() const;
	int								getSelectedLocation() const; // only for testing purposes
	t_server_config					getSelectedServer() const; /// should probably return t_server_config
	int								getStatus() const;
	bool							getResponseStatus() const;
	int								getBytesRead() const;
	int								getRequestLength() const;
	const RequestHeader&			getHeaderInfo();
	CGIHandler*						getCGI();
	std::string						getTempBodyFilepath() const;
	std::string						getIntRedirRefPath() const;
	int								getNumResponseChunks() const;

	bool							getChunksSentCompleteStatus() const;

	// setters
	void							setStatus(int);
	
	// buffer TBD
	unsigned char					buf[BUFFER_SIZE + 1];
	int								buf_pos;
	
	unsigned char					cgi_buf[BUFFER_SIZE + 1];
	int								cgi_buf_pos;
	int								cgi_bytes_read;

	// methods
	void							determineLocationBlock();
	void							processRequest();
	void							sendResponse();
	void							findServerBlock();
	void							findLocationBlock();
	void							checkAllowedMethods();
	void							checkInternalRedirect();
	int								calcMatches(std::vector<std::string>&, std::vector<std::string>&); // make private?
	std::vector<std::string>		splitPath(std::string input, char delim);
	AResponse*						prepareResponse();
	AUploadModule*					checkContentType();
	void							checkForCGI();
	void							setCGIResponse();
	void							readCGIResponse();
	void							removeTempFiles();
	
	
	
	
	
	

	

	// make private?
	enum {
		unknown = 0,
		multipart_form,
		text_plain,
		urlencoded
	} content_type;

};

#endif
