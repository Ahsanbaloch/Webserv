
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

RequestHandler::RequestHandler()
	: request_header(*this)
{
	kernel_q_fd = -1;
	connection_fd = -1;
	status = 200;
	selected_location = 0;
	selected_server = 0;
	bytes_read = 0;
	request_length = 0;
	num_response_chunks_sent = 0;

	response_ready = 0;
	all_chunks_sent = 0;
	cgi_detected = 0;
	header_check_done = 0;

	buf_pos = -1;
	cgi_buf_pos = -1;

	cgi_identifier = NULL;
	chunk_decoder = NULL;
	cgi_handler = NULL;
	uploader = NULL;
	response = NULL;
	body_extractor = NULL;
}

RequestHandler::RequestHandler(int fd, std::vector<t_server_config> server_config, int q_fd)
	: request_header(*this)
{
	this->server_config = server_config;
	kernel_q_fd = q_fd;
	connection_fd = fd;
	status = 200;
	selected_location = 0;
	selected_server = 0;
	bytes_read = 0;
	request_length = 0;
	num_response_chunks_sent = 0;

	response_ready = 0;
	all_chunks_sent = 0;
	cgi_detected = 0;
	header_check_done = 0;

	buf_pos = -1;
	cgi_buf_pos = -1;

	cgi_identifier = NULL;
	chunk_decoder = NULL;
	cgi_handler = NULL;
	uploader = NULL;
	response = NULL;
	body_extractor = NULL;
}

RequestHandler::~RequestHandler()
{
	if (cgi_identifier != NULL)
		delete cgi_identifier;
	if (response != NULL)
		delete response;
	if (uploader != NULL)
		delete uploader;
	if (body_extractor != NULL)
		delete body_extractor;
	if (chunk_decoder != NULL)
		delete chunk_decoder;
	if (cgi_handler != NULL)
	{
		kill(cgi_handler->getCGIPid(), SIGKILL);
		close(cgi_handler->cgi_out[0]);
		delete cgi_handler;
	}
}

RequestHandler::RequestHandler(const RequestHandler& src)
	: request_header(src.request_header)
{
	server_config = src.server_config;
	int_redir_referer_path = src.int_redir_referer_path;
	kernel_q_fd = src.kernel_q_fd;
	connection_fd = src.connection_fd;
	status = src.status;
	selected_location = src.selected_location;
	selected_server = src.selected_server;
	bytes_read = src.bytes_read;
	request_length = src.request_length;
	num_response_chunks_sent = src.num_response_chunks_sent;

	response_ready = src.response_ready;
	all_chunks_sent = src.all_chunks_sent;
	cgi_detected = src.cgi_detected;
	header_check_done = src.header_check_done;

	buf_pos = src.buf_pos;
	cgi_buf_pos = src.cgi_buf_pos;

	cgi_identifier = src.cgi_identifier;
	chunk_decoder = src.chunk_decoder;
	cgi_handler = src.cgi_handler;
	uploader = src.uploader;
	response = src.response;
	body_extractor = src.body_extractor;
}

RequestHandler& RequestHandler::operator=(const RequestHandler& src)
{
	if (this != &src)
	{
		server_config = src.server_config;
		int_redir_referer_path = src.int_redir_referer_path;
		kernel_q_fd = src.kernel_q_fd;
		connection_fd = src.connection_fd;
		status = src.status;
		selected_location = src.selected_location;
		selected_server = src.selected_server;
		bytes_read = src.bytes_read;
		request_length = src.request_length;
		num_response_chunks_sent = src.num_response_chunks_sent;

		response_ready = src.response_ready;
		all_chunks_sent = src.all_chunks_sent;
		cgi_detected = src.cgi_detected;
		header_check_done = src.header_check_done;

		buf_pos = src.buf_pos;
		cgi_buf_pos = src.cgi_buf_pos;

		cgi_identifier = src.cgi_identifier;
		chunk_decoder = src.chunk_decoder;
		cgi_handler = src.cgi_handler;
		uploader = src.uploader;
		response = src.response;
		body_extractor = src.body_extractor;
	}
	return (*this);
}


///////// GETTERS ///////////

const RequestHeader&	RequestHandler::getHeaderInfo()
{
	return (request_header);
}

ChunkDecoder*	RequestHandler::getChunkDecoder() const
{
	return (chunk_decoder);
}

CGIHandler*	RequestHandler::getCGI()
{
	return (cgi_handler);
}

AUploadModule*	RequestHandler::getUploader() const
{
	return (uploader);
}

std::vector<t_server_config>	RequestHandler::getServerConfig() const
{
	return (server_config);
}

s_location_config	RequestHandler::getLocationConfig() const
{
	return (server_config[selected_server].locations[selected_location]);
}

t_server_config	RequestHandler::getSelectedServer() const
{
	return (server_config[selected_server]);
}

std::string	RequestHandler::getIntRedirRefPath() const
{
	return (int_redir_referer_path);
}

std::string	RequestHandler::getTempBodyFilepath() const
{
	return (body_extractor->getTempBodyFilepath());
}

int	RequestHandler::getStatus() const
{
	return (status);
}

int		RequestHandler::getBytesRead() const
{
	return (bytes_read);
}

int	RequestHandler::getCGIBytesRead() const
{
	return (cgi_bytes_read);
}

int		RequestHandler::getRequestLength() const
{
	return (request_length);
}

int	RequestHandler::getNumResponseChunks() const
{
	return (num_response_chunks_sent);
}

bool	RequestHandler::getResponseStatus() const
{
	return (response_ready);
}

bool	RequestHandler::getChunksSentCompleteStatus() const
{
	return (all_chunks_sent);
}


///////// SETTERS ///////////

void	RequestHandler::setStatus(int status)
{
	this->status = status;
}


////////// HELPER METHODS ////////

void	RequestHandler::receiveRequest()
{
	buf_pos = -1;
	bytes_read = recv(connection_fd, buf, BUFFER_SIZE, 0);
	if (bytes_read == -1)
		throw CustomException("Failed to receive request. Connection will be closed");
	if (bytes_read == 0)
		throw CustomException("Stream socket peer has performed an orderly shutdown. Connection will be closed");
	request_length += bytes_read;
	buf[bytes_read] = '\0';
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
	#ifdef __APPLE__
		struct kevent cgi_event;
		int* ident = new int;
		*ident = connection_fd;
		if (fcntl(cgi_handler->cgi_out[0], F_SETFL, O_NONBLOCK) == -1)
			throw CustomException("Failed when calling fcntl() and setting fds to non-blocking");
		EV_SET(&cgi_event, cgi_handler->cgi_out[0], EVFILT_READ, EV_ADD, 0, 0, ident);
		if (kevent(kernel_q_fd, &cgi_event, 1, NULL, 0, NULL) == -1)
			throw CustomException("Failed when registering events for CGI output");
	#else
		struct epoll_event listening_event;
		EPoll::e_data* user_data = new EPoll::e_data;
		user_data->fd = connection_fd;
		user_data->socket_ident = 3;
		listening_event.data.ptr = user_data;
		listening_event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
		if (fcntl(cgi_handler->cgi_out[0], F_SETFL, O_NONBLOCK) == -1)
			throw CustomException("Failed when calling fcntl() and setting fds to non-blocking");
		if (epoll_ctl(kernel_q_fd, EPOLL_CTL_ADD, cgi_handler->cgi_out[0], &listening_event) == -1)
			throw CustomException("Failed when registering events for CGI output");
	#endif
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
	std::vector<std::string> location_extensions = getLocationConfig().cgi_ext;
	if (getLocationConfig().path == "/cgi-bin")
	{
		if (find(location_extensions.begin(), location_extensions.end(), request_header.getFileExtension()) == location_extensions.end())
		{
			status = 403;
			throw CustomException("Forbidden: specified cgi file extension is not allowed");
		}
		else
		{
			std::string cgi_path = getLocationConfig().root + request_header.getPath();
			if (access(cgi_path.c_str(), F_OK) == 0)
				cgi_detected = 1;
			else
			{
				setStatus(404);
				throw CustomException("Not found: cgi file");
			}
		}
	}
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
		std::string new_file_path = getLocationConfig().index;
		if (access(new_file_path.c_str(), F_OK) == 0)
		{
			int_redir_referer_path = "http://localhost:" + toString(getSelectedServer().port) + getLocationConfig().path;
			new_file_path = new_file_path.substr(getLocationConfig().root.length());
			request_header.makeInternalRedirect(new_file_path);
			findLocationBlock();
			checkAllowedMethods();
		}
	}
}

void RequestHandler::determineLocationBlock()
{
	if (server_config.size() > 1)
		findServerBlock();
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
	else if (type == "text/plain")
	{
		content_type = text_plain;
		return (new UploadPlain(*this));
	}
	else
	{
		setStatus(415);
		throw CustomException("Unsupported content type");
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

std::string	RequestHandler::fetchResponseChunk()
{
	std::string resp;

	if (response->getChunkedBodyStatus() && status < 400)
	{
		if (num_response_chunks_sent > 0)
			resp = response->createBodyChunk();
		else
			resp = response->getResponseStatusLine() + response->getRespondsHeaderFields() + response->getResponseBody();
		num_response_chunks_sent++;
	}
	else
		resp = response->getResponseStatusLine() + response->getRespondsHeaderFields() + response->getResponseBody();
	
	return (resp);
}

void	RequestHandler::makeInternalRedirectPostCGI(std::string location)
{
	close(cgi_handler->cgi_out[0]);
	request_header.makeInternalRedirect(location);
	findLocationBlock();
	checkInternalRedirect();
	delete response;
	response = new GETResponse(*this);
	response->createResponse();
	if (getHeaderInfo().getMethod() == "POST")
		remove(getTempBodyFilepath().c_str());
	response_ready = 1;
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
			if (request_header.getBodyExpectanceStatus() && getLocationConfig().redirect.empty())
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
		if (cgi_handler != NULL)
			close(cgi_handler->cgi_out[0]);
		makeErrorResponse();
		std::cerr << e.what() << '\n';
	}
}

void	RequestHandler::sendResponse()
{
	std::string resp;
	try
	{
		resp = fetchResponseChunk();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		makeErrorResponse();
	}
	
	int bytes_sent = send(connection_fd, resp.c_str(), resp.length(), 0);
	if (bytes_sent == -1)
		throw CustomException("Error when sending data. Connection will be closed");
	if (bytes_sent == 0)
		throw CustomException("Client not able to receive response. Connection will be closed");
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
			throw CustomException("CGI read failed");
		else if (cgi_bytes_read == 0)
		{
			close(cgi_handler->cgi_out[0]);
			response->createResponse();
			if (getHeaderInfo().getMethod() == "POST")
			{
				remove(getTempBodyFilepath().c_str());
				CGIResponse* cgiResponse = dynamic_cast<CGIResponse*>(response);
				if (cgiResponse != NULL)
				{
					std::map<std::string, std::string> temp = cgiResponse->getCGIHeaderFields();
					if (temp.find("Content-Type") != temp.end())
						remove(cgiResponse->getTempBodyFilePath().c_str());
				}
			}
			else if (getHeaderInfo().getMethod() == "GET")
			{
				CGIResponse* cgiResponse = dynamic_cast<CGIResponse*>(response);
				if (cgiResponse != NULL)
					remove(cgiResponse->getTempBodyFilePath().c_str());
			}
			response_ready = 1;
		}
		else
		{
			cgi_buf[cgi_bytes_read] = '\0';
			CGIResponse* cgiResponse = dynamic_cast<CGIResponse*>(response);
			if (cgiResponse != NULL)
			{
				if (cgiResponse->processBuffer())
					makeInternalRedirectPostCGI(cgiResponse->getCGIHeaderFields()["Location"]);
			}
		}
	}
	catch(const std::exception& e)
	{
		close(cgi_handler->cgi_out[0]);
		makeErrorResponse();
		std::cerr << e.what() << '\n';
	}
}

void	RequestHandler::initCGIResponse()
{
	if (response == NULL)
		response = new CGIResponse(*this);
}
