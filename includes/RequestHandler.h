#ifndef REQUESTHANDLER_H
# define REQUESTHANDLER_H

#ifdef __APPLE__
	# include <sys/event.h>
#else
	# include <sys/epoll.h> 
	# include "EPoll.h"
#endif
# include <sys/socket.h>
# include <unistd.h>
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

	// vars
	std::vector<t_server_config>	server_config;
	std::string						int_redir_referer_path;
	int* 							cgi_identifier;
	int								kernel_q_fd;
	int								connection_fd;
	int								status;
	int								selected_location;
	int								selected_server;
	int								bytes_read;
	int								cgi_bytes_read;
	int								request_length;
	int								num_response_chunks_sent;

	// flags
	bool							response_ready;
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
	void							checkForCGI();
	void							checkAllowedMethods();
	void							checkInternalRedirect();
	void							determineLocationBlock();
	void							findServerBlock();
	void							removeTempFiles();
	void							findLocationBlock();
	void							makeInternalRedirectPostCGI(std::string);
	int								calcMatches(std::vector<std::string>&, std::vector<std::string>&);
	std::string						fetchResponseChunk();
	AResponse*						prepareResponse();
	AUploadModule*					checkContentType();

	// constructors
	RequestHandler(const RequestHandler&);

public:
	// constructors & destructors
	RequestHandler();
	RequestHandler(int, std::vector<t_server_config>, int);
	RequestHandler& operator=(const RequestHandler&);
	~RequestHandler();

	// getters
	const RequestHeader&			getHeaderInfo();
	ChunkDecoder*					getChunkDecoder() const;
	CGIHandler*						getCGI();
	AUploadModule*					getUploader() const;

	std::vector<t_server_config>	getServerConfig() const;
	s_location_config				getLocationConfig() const;
	t_server_config					getSelectedServer() const;
	std::string						getIntRedirRefPath() const;
	std::string						getTempBodyFilepath() const;
	int								getStatus() const;
	int								getBytesRead() const;
	int								getCGIBytesRead() const;
	int								getRequestLength() const;
	int								getNumResponseChunks() const;

	bool							getResponseStatus() const;
	bool							getChunksSentCompleteStatus() const;

	// setters
	void							setStatus(int);
	
	// buffers
	unsigned char					buf[BUFFER_SIZE + 1];
	int								buf_pos;
	unsigned char					cgi_buf[BUFFER_SIZE + 1];
	int								cgi_buf_pos;

	// main methods
	void							processRequest();
	void							sendResponse();
	void							readCGIResponse();
	void							initCGIResponse();

	// states
	enum {
		unknown = 0,
		multipart_form,
		text_plain,
		urlencoded
	} content_type;
};

#endif
