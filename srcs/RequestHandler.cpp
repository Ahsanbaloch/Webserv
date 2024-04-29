
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

RequestHandler::RequestHandler()
	: request_header(*this), Q(*new KQueue())
{
}

RequestHandler::RequestHandler(int fd, std::vector<t_server_config> server_config,  const KQueue& q)
	: request_header(*this), Q(q)
{
	std::cout << "request handler constructed" << std::endl;
	this->server_config = server_config;
	connection_fd = fd;
	status = 200;
	selected_location = 0;
	selected_server = 0;
	bytes_read = 0;
	response_ready = 0;
	request_length = 0;
	internal_redirect = 0;
	num_response_chunks_sent = 0;
	all_chunks_sent = 0;
	cgi_detected = 0;
	// total_bytes_sent = 0;

	header_check_done = 0;

	buf_pos = -1;
	cgi_buf_pos = -1;

	 // also add in copy constructor etc.

	
	response = NULL;
	uploader = NULL;
	body_extractor = NULL;
	cgi_handler = NULL;
	chunk_decoder = NULL;
	memset(&buf, 0, sizeof(buf));
}

RequestHandler::~RequestHandler()
{
	std::cout << "request handler destroyed" << std::endl;
	delete response;
	if (uploader != NULL) // needed?
		delete uploader;
	if (body_extractor != NULL)
		delete body_extractor;
	if (chunk_decoder != NULL)
		delete chunk_decoder;
	if (cgi_handler != NULL)
		delete cgi_handler;
}

RequestHandler::RequestHandler(const RequestHandler& src)
	: request_header(src.request_header), Q(src.Q)
{
	// tbd
}

RequestHandler& RequestHandler::operator=(const RequestHandler& src)
{
	if (this != &src)
	{
		request_header = src.request_header;
		server_config = src.server_config;
		status = src.status;
		selected_location = src.selected_location;
		selected_server = src.selected_server;
		connection_fd = src.connection_fd;
		bytes_read = src.bytes_read;
		response_ready = src.response_ready;
		request_length = src.request_length;
		internal_redirect = src.internal_redirect;
		num_response_chunks_sent = src.num_response_chunks_sent;
		buf_pos = src.buf_pos;
		response = src.response;
		uploader = src.uploader;
		body_extractor = src.body_extractor;
	}
	return (*this);
}


///////// GETTERS ///////////

std::vector<t_server_config>	RequestHandler::getServerConfig() const
{
	return (server_config);
}

int	RequestHandler::getStatus() const
{
	return (status);
}

s_location_config	RequestHandler::getLocationConfig() const
{
	return (server_config[selected_server].locations[selected_location]);
}

int	RequestHandler::getSelectedLocation() const
{
	return (selected_location);
}

bool	RequestHandler::getResponseStatus() const
{
	return (response_ready);
}

int		RequestHandler::getBytesRead() const
{
	return (bytes_read);
}

int		RequestHandler::getRequestLength() const
{
	return (request_length);
}

const RequestHeader&	RequestHandler::getHeaderInfo()
{
	return (request_header);
}

CGIHandler*	RequestHandler::getCGI()
{
	return (cgi_handler);
}

t_server_config	RequestHandler::getSelectedServer() const
{
	return (server_config[selected_server]);
}

std::string	RequestHandler::getTempBodyFilepath() const
{
	return (body_extractor->getTempBodyFilepath());
}

std::string	RequestHandler::getIntRedirRefPath() const
{
	return (int_redir_referer_path);
}

AUploadModule*	RequestHandler::getUploader() const
{
	return (uploader);
}

int	RequestHandler::getNumResponseChunks() const
{
	return (num_response_chunks_sent);
}

bool	RequestHandler::getChunksSentCompleteStatus() const
{
	return (all_chunks_sent);
}

// provide a function to get the chunk decoder filepath instead?
ChunkDecoder*	RequestHandler::getChunkDecoder() const
{
	return (chunk_decoder);
}


///////// SETTERS ///////////

void	RequestHandler::setStatus(int status)
{
	this->status = status;
}


////////// HELPER METHODS ////////

void	RequestHandler::receiveRequest()
{
	memset(buf, 0, sizeof(buf));
	buf_pos = -1;
	bytes_read = recv(connection_fd, buf, BUFFER_SIZE, 0);
	if (bytes_read == -1)
		throw CustomException("Recv failed. Close connection\n");
	if (bytes_read == 0)
		throw CustomException("Client disconnected\n");
	request_length += bytes_read;

	// printf("read %i bytes\n", bytes_read);
	// printf("buffer content: \n%s\n", buf);
}

void	RequestHandler::processHeader()
{
	if (!request_header.getRequestLineStatus())
		request_header.parseRequestLine();
	if (request_header.getRequestLineStatus())
	{
		request_header.identifyFileName();
		determineLocationBlock();
		checkAllowedMethods();
		if (request_header.getMethod() == "GET")
			checkInternalRedirect();
		request_header.parseHeaderFields();
	}
}

void	RequestHandler::checkHeader()
{
	request_header.checkHeader();
	checkForCGI();
	if (request_header.getHeaderExpectedStatus()) // this is relevant for POST only, should this be done in another place? (e.g. POST request class)
	{
		// check value of expect field?
		// check content-length field before accepting?
		// create response signalling the client that the body can be send
		// make reponse
		// in this case we don't want to destroy the requesthandler object
	}
	header_check_done = 1;
}

void	RequestHandler::processBody()
{
	if (getHeaderInfo().getTEStatus() && (chunk_decoder == NULL || !chunk_decoder->getDecodingStatus()))
	{
		if (chunk_decoder == NULL)
			chunk_decoder = new ChunkDecoder(*this);
		chunk_decoder->unchunkBody();
	}
	if (!getHeaderInfo().getTEStatus() || (chunk_decoder != NULL && chunk_decoder->getDecodingStatus()))
	{
		if (cgi_detected)
		{
			if (body_extractor == NULL)
				body_extractor = new BodyExtractor(*this);
			body_extractor->extractBody();
		}
		else
		{
			if (uploader == NULL)
				uploader = checkContentType();
			uploader->uploadData();
		}
	}
}

void	RequestHandler::addCGIToQueue()
{
	cgi_handler = new CGIHandler(*this);
	struct kevent cgi_event;
	if (fcntl(cgi_handler->cgi_out[0], F_SETFL, O_NONBLOCK) == -1)
		throw CustomException("Failed when calling fcntl() and setting fds to non-blocking\n");
	EV_SET(&cgi_event, cgi_handler->cgi_out[0], EVFILT_READ, EV_ADD, 0, 0, &connection_fd);
	if (kevent(Q.getKQueueFD(), &cgi_event, 1, NULL, 0, NULL) == -1)
		throw CustomException("Failed when registering events for CGI output\n");
}

void	RequestHandler::makeErrorResponse()
{
	removeTempFiles();
	if (response != NULL)
		delete response;
	response = new ERRORResponse(*this);
	response->createResponse();
	response_ready = 1;
}

void	RequestHandler::checkForCGI()
{
	if (getLocationConfig().path == "/cgi-bin")
	{
		if (find(getLocationConfig().cgi_ext.begin(), getLocationConfig().cgi_ext.end(), request_header.getFileExtension()) == getLocationConfig().cgi_ext.end())
		{
			status = 403; // which status should be set here? 
			throw CustomException("Forbidden");
		}
		else
			cgi_detected = 1;
	}
	// also check execution rights here?
}

void	RequestHandler::checkAllowedMethods()
{
	if (request_header.getMethod() == "GET" && !getLocationConfig().GET)
	{
		setStatus(405);
		throw CustomException("Method Not Allowed");
	}
	else if (request_header.getMethod() == "POST" && !getLocationConfig().POST)
	{
		setStatus(405);
		throw CustomException("Method Not Allowed");
	}
	else if (request_header.getMethod() == "DELETE" && !getLocationConfig().DELETE)
	{
		setStatus(405);
		throw CustomException("Method Not Allowed");
	}
}

void RequestHandler::checkInternalRedirect()
{
	if (request_header.getFileExtension().empty())
	{
		std::cout << "initial path: " << request_header.getPath() << std::endl;
		std::string new_file_path = getLocationConfig().index;
		if (access(new_file_path.c_str(), F_OK) == 0)
		{
			internal_redirect = 1;
			int_redir_referer_path = "http://localhost:" + toString(getSelectedServer().port) + getLocationConfig().path;
			std::string	orig_root = getLocationConfig().root;
			findLocationBlock();
			checkAllowedMethods();
			if (!getLocationConfig().redirect.empty())
				return ;
			new_file_path = new_file_path.substr(orig_root.length());
			request_header.makeInternalRedirect(new_file_path);
			std::cout << "new path: " << request_header.getPath() << std::endl;
		}
	}
}

void RequestHandler::determineLocationBlock()
{
	// find server block if there are multiple that match (this applies to all request types)
	if (server_config.size() > 1)
		findServerBlock();
	
	// find location block within server block if multiple exist (this applies to all request types; for GET requests there might be an internal redirect happening later on)
	if (server_config[selected_server].locations.size() > 1)
		findLocationBlock();
}

AResponse* RequestHandler::prepareResponse()
{
	if (!getLocationConfig().redirect.empty())
		return (new REDIRECTResponse(*this));
	else if (request_header.getMethod() == "GET")
		return (new GETResponse(*this));
	else if (request_header.getMethod() == "DELETE")
		return (new DELETEResponse(*this));
	else if (request_header.getMethod() == "POST")
		return (new POSTResponse(*this));
	else
	{
		setStatus(501);
		throw CustomException("Not implemented");
	}
}

AUploadModule*	RequestHandler::checkContentType()
{
	std::string value = getHeaderInfo().getHeaderFields()["content-type"];
	std::string type;
	
	// identify content type
	std::size_t delim_pos = value.find(';');
	if (delim_pos != std::string::npos)
		type = value.substr(0, delim_pos);
	else
		type = value;
	
	if (type == "multipart/form-data")
	{
		content_type = multipart_form;
		return (new UploadMultipart(*this));
	}
	else if (type == "application/x-www-form-urlencoded")
	{
		content_type = urlencoded;
		return (new UploadUrlencoded(*this));
	}
	else
	{
		// or throwException if type is not supported?
		content_type = text_plain;
		return (new UploadPlain(*this));
	}
}

void	RequestHandler::findLocationBlock()
{
	std::vector<std::string> uri_path_items = splitPath(request_header.getPath(), '/');
	int	size = server_config[selected_server].locations.size();
	int	max = 0;
	for (int i = 0; i < size; i++)
	{
		std::vector<std::string> location_path_items = splitPath(server_config[selected_server].locations[i].path, '/');
		int matches = calcMatches(uri_path_items, location_path_items);
		if (matches > max)
		{
			selected_location = i;
			max = matches;
		}
	}
}

int	RequestHandler::calcMatches(std::vector<std::string>& uri_path_items, std::vector<std::string>& location_path_items)
{
	int	matches = 0;
	int num_path_items = uri_path_items.size();
	for (std::vector<std::string>::iterator it = location_path_items.begin(); it != location_path_items.end(); it++)
	{
		if (matches >= num_path_items)
			break;
		if (*it != uri_path_items[matches])
			break;
		matches++;
	}
	return (matches);
}

void	RequestHandler::findServerBlock()
{
	int size = server_config.size();

	for (int i = 0; i < size; i++)
	{
		if (server_config[i].serverName == request_header.getHeaderFields()["host"])
		{
			selected_server = i;
			break;
		}
	}
}

void	RequestHandler::removeTempFiles()
{
	if (body_extractor != NULL && !getTempBodyFilepath().empty())
		remove(getTempBodyFilepath().c_str());
	if (chunk_decoder != NULL && !chunk_decoder->getUnchunkedDataFile().empty())
		remove(chunk_decoder->getUnchunkedDataFile().c_str());
	CGIResponse* cgiResponse = dynamic_cast<CGIResponse*>(response);
	if (cgiResponse != NULL)
	{
		if (!cgiResponse->getTempBodyFilePath().empty())
			remove(cgiResponse->getTempBodyFilePath().c_str());
	}
}


///////// MAIN METHODS ///////////

void	RequestHandler::processRequest()
{
	receiveRequest();
	try
	{
		if (!request_header.getHeaderProcessingStatus())
			processHeader();
		if (request_header.getHeaderProcessingStatus())
		{
			if (!header_check_done)
				checkHeader();			
			if (request_header.getBodyExpectanceStatus() && request_header.getMethod() == "POST" && getLocationConfig().redirect.empty())
				processBody();
			if (!request_header.getBodyExpectanceStatus() || (uploader != NULL && uploader->getUploadStatus())
				|| !getLocationConfig().redirect.empty() || (body_extractor != NULL && body_extractor->getExtractionStatus()))
			{
				if (cgi_detected)
				{
					addCGIToQueue();
					cgi_handler->execute();
					return ;
				}
				response = prepareResponse();
				response->createResponse();
				response_ready = 1;
			}
		}
	}
	catch(const std::exception& e)
	{
		makeErrorResponse();
		std::cerr << e.what() << '\n';
	}
}

void	RequestHandler::sendResponse()
{
	std::string resp;

	if (response->getChunkedBodyStatus() && status < 400)
	{
		if (num_response_chunks_sent > 0)
		{
			std::string body_chunk;
			body_chunk = response->createBodyChunk();
			resp = body_chunk;
		}
		else
			resp = response->getResponseStatusLine() + response->getRespondsHeaderFields() + response->getResponseBody();
		num_response_chunks_sent++;
	}
	else
		resp = response->getResponseStatusLine() + response->getRespondsHeaderFields() + response->getResponseBody();
	
	int bytes_sent = send(connection_fd, resp.c_str(), resp.length(), 0);
	if (bytes_sent == -1)
	{
		throw CustomException("Error when sending data. Closing connection");
	}
	if (bytes_sent == 0)
	{
		throw CustomException("Connection closed");
	}
	if (response->getChunkedBodyStatus())
	{
		if (num_response_chunks_sent == 1)
			bytes_sent -= response->getResponseStatusLine().length() + response->getRespondsHeaderFields().length();
		if (bytes_sent > 0)
			response->incrementFilePosition(bytes_sent);
		if (response->getFilePosition() - response->getFilePosOffset() == response->getBodySize())
		{
			all_chunks_sent = 1;
			response->getBodyFile().close();
		}
	}
}

void	RequestHandler::readCGIResponse()
{
	try
	{
		cgi_buf_pos = -1;
		cgi_bytes_read = read(cgi_handler->cgi_out[0], cgi_buf, BUFFER_SIZE);
		if (cgi_bytes_read == -1)
			perror("recv");
		else if (cgi_bytes_read == 0)
		{
			close(cgi_handler->cgi_out[0]);
			response->createResponse();
			if (getHeaderInfo().getMethod() == "POST")
				remove(getTempBodyFilepath().c_str());
			response_ready = 1;
		}
		else
		{
			cgi_buf[bytes_read] = '\0';

			CGIResponse* cgiResponse = dynamic_cast<CGIResponse*>(response);
			if (cgiResponse != NULL)
			{
				if (cgiResponse->processBuffer())
				{
					close(cgi_handler->cgi_out[0]);
					request_header.makeInternalRedirect(cgiResponse->cgi_header_fields["Location"]);
					findLocationBlock();
					delete response;
					response = new GETResponse(*this);
					response->createResponse();
					if (getHeaderInfo().getMethod() == "POST")
						remove(getTempBodyFilepath().c_str());
					response_ready = 1;
				}
			}
		}
	}
	catch(const std::exception& e)
	{
		makeErrorResponse();
		std::cerr << e.what() << '\n';
	}
}

void	RequestHandler::initCGIResponse()
{
	if (response == NULL)
		response = new CGIResponse(*this);
}

std::vector<std::string>	RequestHandler::splitPath(std::string input, char delim)
{
	std::istringstream			iss(input);
	std::string					item;
	std::vector<std::string>	result;
	
	while (std::getline(iss, item, delim))
		result.push_back("/" + item);

	return (result);
}
