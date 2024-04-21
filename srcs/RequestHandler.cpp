
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

RequestHandler::RequestHandler()
	: request_header(*this)
{
}

RequestHandler::RequestHandler(int fd, std::vector<t_server_config> server_config)
	: request_header(*this)
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

	buf_pos = -1;

	 // also add in copy constructor etc.
	chunk_length = 0;
	total_chunk_size = 0;
	trailer_exists = 0;
	body_unchunked = 0;
	te_state = body_start;
	
	response = NULL;
	uploader = NULL;
	body_extractor = NULL;
	memset(&buf, 0, sizeof(buf));
}

RequestHandler::~RequestHandler()
{
	std::cout << "request handler destroyed" << std::endl;
	delete response;
	delete uploader;
	if (body_extractor != NULL) // needed?
		delete body_extractor;
}

RequestHandler::RequestHandler(const RequestHandler& src)
	: request_header(src.request_header)
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

int	RequestHandler::getSelectedServer() const
{
	return (selected_server);
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

bool	RequestHandler::getUnchunkingStatus() const
{
	return (body_unchunked);
}

std::string	RequestHandler::getTempBodyFilepath() const
{
	return (body_extractor->getTempBodyFilepath());
}



///////// SETTERS ///////////

void	RequestHandler::setStatus(int status)
{
	this->status = status;
}


///////// METHODS ///////////

void	RequestHandler::sendResponse()
{
	std::string resp = response->getResponseStatusLine() + response->getRespondsHeaderFields() + response->getResponseBody();
	// std::cout << resp << std::endl;
	send(connection_fd, resp.c_str(), resp.length(), 0); 
	// check for errors when calling send
}

void	RequestHandler::processRequest()
{
	buf_pos = -1;
	bytes_read = recv(connection_fd, buf, BUFFER_SIZE, 0);
	if (bytes_read == -1)
		throw CustomException("Failed when processing read request\n");
	if (bytes_read == 0)
		return ; // also throw exception. Need to check the exception exactly // also close connection?
	// close fd in case bytes_read == 0 ???
	
	buf[bytes_read] = '\0';
	request_length += bytes_read;

	printf("read %i bytes\n", bytes_read);
	printf("buffer content: \n%s\n", buf);

	try
	{
		// check if headers have already been read
		if (!request_header.getHeaderStatus())
		{
			if (!request_header.getRequestLineStatus())
				request_header.parseRequestLine();
			if (request_header.getRequestLineStatus())
			{
				determineLocationBlock();
				checkAllowedMethods(); // check if method is allowed in selected location
				request_header.parseHeaderFields();
			}
		}

		if (request_header.getHeaderStatus())
		{
			request_header.checkHeader();

			//for testing: print received headers
			printf("\nheaders\n");
			std::map<std::string, std::string> headers = request_header.getHeaderFields();
			for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
			{
				std::cout << "key: " << it->first << " ";
				std::cout << "value: " << it->second << std::endl;
			}

			if (request_header.getHeaderExpectedStatus()) // this is relevant for POST only, should this be done in another place? (e.g. POST request class)
			{
				// check value of expect field?
				// check content-length field before accepting?
				// create response signalling the client that the body can be send
				// make reponse
				// in this case we don't want to destroy the requesthandler object
			}
			// if body is expected, read the body (unless the selected location demands a redirect or it is not a POST request) 
			if (request_header.getBodyStatus() && request_header.getMethod() == "POST" && getLocationConfig().redirect.empty())
			{
				// unchunk body if needed
				if (getHeaderInfo().getTEStatus() && !body_unchunked)
					unchunkBody();
				if (!getHeaderInfo().getTEStatus() || body_unchunked)
				{
					if (request_header.getFileExtension() == ".py") // what if other cgi extension?
					{
						if (body_extractor == NULL)
							body_extractor = new BodyExtractor(*this);
						body_extractor->extractBody();
						std::cout << body_extractor->getTempBodyFilepath() << std::endl;
					}
					else
					{
						if (uploader == NULL)
							uploader = checkContentType(); // needs to be deleted/freed somewhere
						uploader->uploadData();
					}
				}
			}
			// if no body is expected OR end of body has been reached
			// check how getBodyStatus() gets set (does it already check for request method?)
			if (!request_header.getBodyStatus() || (uploader != NULL && uploader->getUploadStatus())
				|| !getLocationConfig().redirect.empty() || (body_extractor != NULL && body_extractor->getExtractionStatus()))
			{
				// std::cout << "body content: " << request_body.body << std::endl;
				response = prepareResponse(); // how to handle errors in here?
				response->createResponse(); // how to handle errors in here?
				std::cout << "bug?: " << getServerConfig()[0].port << std::endl;
				// if index directive is cgi, check here again and create cgi object
				response_ready = 1;
			}
		}
	}
	catch(const std::exception& e)
	{
		// delete response before reassigning if response != NULL ?
		response = new ERRORResponse(*this); // need to free this somewhere
		response->createResponse();
		response_ready = 1;
		std::cerr << e.what() << '\n';
	}
}

void		RequestHandler::checkAllowedMethods()
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
		return (new GETResponse(*this)); // need to free this somewhere
	else if (request_header.getMethod() == "DELETE")
		return (new DELETEResponse(*this)); // need to free this somewhere
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
	std::vector<std::string> uri_path_items;
	if (response == NULL || !response->getInternalRedirectStatus())
		uri_path_items = splitPath(request_header.getPath(), '/');
	else
	{
		std::string temp = response->getFullFilePath().substr(getLocationConfig().root.length());
		uri_path_items = splitPath(temp, '/');
	}
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
	std::cout << "Thats the location block: " << getLocationConfig().path << std::endl;
}

std::vector<std::string>	RequestHandler::splitPath(std::string input, char delim)
{
	std::istringstream			iss(input);
	std::string					item;
	std::vector<std::string>	result;
	
	while (std::getline(iss, item, delim))
		result.push_back("/" + item); // does adding "/" work in all cases?
	// if (result.size() == 1 && result[0].empty())
	// 	result[0] = '/';
	return (result);
}

int	RequestHandler::calcMatches(std::vector<std::string>& uri_path_items, std::vector<std::string>& location_path_items)
{
	// printf("splitted string\n");
	// for (std::vector<std::string>::iterator it = uri_path_items.begin(); it != uri_path_items.end(); it++)
	// {
	// 	std::cout << "string uri: " << *it << std::endl;
	// }
	// for (std::vector<std::string>::iterator it = location_path_items.begin(); it != location_path_items.end(); it++)
	// {
	// 	std::cout << "string location: " << *it << std::endl;
	// }
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
	// std::vector<t_server_config>::iterator it = handler.getServerConfig().begin();
	// for (std::vector<t_server_config>::iterator it2 = handler.getServerConfig().begin(); it2 != handler.getServerConfig().end(); it2++)
	// {
	// 	if (it2 == it || it2->serverName == handler.header.header_fields["host"])
	// 		it = it2;
	// 	else
	// 	{
	// 		handler.getServerConfig().erase(it2);
	// 		it2--;
	// 	}
	// }
	// if (it != handler.getServerConfig().begin())
	// 	handler.getServerConfig().erase(handler.getServerConfig().begin());
}



std::string	RequestHandler::getUnchunkedDataFile() const
{
	return (temp_filename_unchunked);
}

int			RequestHandler::getTotalChunkSize() const
{
	return (total_chunk_size);
}

void	RequestHandler::storeChunkedData()
{
	if (temp_filename_unchunked.empty())
	{
		std::ostringstream num_conversion;
		g_num_temp_unchunked_files++;
		num_conversion << g_num_temp_unchunked_files;
		temp_filename_unchunked = "www/temp/" + num_conversion.str() + ".bin";
	}

	temp_unchunked.open(temp_filename_unchunked, std::ios::app | std::ios::binary);
	int to_write = std::min(getBytesRead() - buf_pos, chunk_length);
	temp_unchunked.write(reinterpret_cast<const char*>(&buf[buf_pos]), to_write);
	buf_pos += to_write;
	chunk_length -= to_write;
	temp_unchunked.close();

}

void	RequestHandler::unchunkBody()
{
	while (!body_unchunked && buf_pos++ < getBytesRead())
	{
		unsigned char ch = buf[buf_pos];

		switch (te_state)
		{
			case body_start:
				if ((ch >= '0' && ch <= '9'))
				{
					chunk_length = ch - '0';
					te_state = chunk_size;
					break;
				}
				else if (ch >= 'a' && ch <= 'f')
				{
					chunk_length = ch - 'a' + 10;
					te_state = chunk_size;
					break;
				}
				else if (ch >= 'A' && ch <= 'F')
				{
					chunk_length = ch - 'A' + 10;
					te_state = chunk_size;
					break;
				}
				else
				{
					setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 1"); // other exception?
				}

			case chunk_size:
				if (ch >= '0' && ch <= '9')
				{
					chunk_length = chunk_length * 16 + (ch - '0');
					break;
				}
				else if (ch >= 'a' && ch <= 'f')
				{
					chunk_length = chunk_length * 16 + (ch - 'a' + 10);
					break;
				}
				else if (ch >= 'A' && ch <= 'F')
				{
					chunk_length = chunk_length * 16 + (ch - 'A' + 10);
					break;
				}
				else if (chunk_length == 0) // how to end if body is distributed over multiple requests?
				{
					if (ch == CR)
					{
						te_state = chunk_size_cr;
						break;
					}
					else if (ch == LF)
					{
						te_state = chunk_trailer;
						break;
					}
					else if (ch == ';')
					{
						te_state = chunk_extension; // are there more seperators? // seperate state for last extension?
						break;
					}
					else
					{
						setStatus(400); // what is the correct error code?
						throw CustomException("Bad request 2"); // other exception?
					}
				}
				else if (ch == CR)
				{
					total_chunk_size += chunk_length;
					te_state = chunk_size_cr;
					break;
				}
				else if (ch == LF)
				{
					total_chunk_size += chunk_length;
					te_state = chunk_data;
					break;
				}
				else if (ch == ';')
				{
					total_chunk_size += chunk_length;
					te_state = chunk_extension;
					break;
				}
				else
				{
					setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 3"); // other exception?
				}
			
			case chunk_size_cr:
				if (ch == LF && chunk_length > 0)
				{
					te_state = chunk_data;
					break;
				}
				else if (ch == LF && chunk_length == 0)
				{
					te_state = chunk_trailer;
					break;
				}
				else
				{
					setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 4"); // other exception?
				}

			case chunk_extension: // skip over chunk_extension // could also be done in a for loop // limit max size of chunk extension -- vulnerabilities
			// A server ought to limit the total length of chunk extensions received in a request to an amount reasonable for the services provided, in the same way that it applies length limitations and timeouts for other parts of a message, and generate an appropriate 4xx (Client Error) response if that amount is exceeded
				if (ch == CR)
				{
					te_state = chunk_size_cr;
					break;
				}
				else if (ch == LF && chunk_length > 0)
				{
					te_state = chunk_data;
					break;
				}
				else if (ch == LF && chunk_length == 0)
				{
					te_state = chunk_trailer;
					break;
				}
				else
					break;

			case chunk_data: // Limit for chunk length?
				storeChunkedData();
				ch = buf[buf_pos];
				if (ch == CR)
				{
					te_state = chunk_data_cr;
					break;
				}
				else if (ch == LF)
				{
					te_state = body_start;
					break;
				}
				break;
				// else
				// {
				// 	handler.setStatus(400); // what is the correct error code?
				// 	throw CustomException("Bad request 5"); // other exception?
				// }

			case chunk_data_cr:
				if (ch == LF)
				{
					te_state = body_start;
					break;
				}
				else
				{
					setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 6"); // other exception?
				}
			
			// is the existence of trailers indicated in the headers
			case chunk_trailer:
				if (ch == CR)
				{
					te_state = chunk_trailer_cr;
					break;
				}
				else if (ch == LF)
				{
					te_state = body_end;
					break;
				}
				else // maybe skip trailer in a for loop? // limit size of trailer?
				{
					trailer_exists = 1; // create loop so that this is not set every time
					break;
				}
				// trailer fields can be useful for supplying message integrity checks, digital signatures, delivery metrics, or post-processing status information
				// probably can just discard this section --> how to identify end?

			case chunk_trailer_cr:
				if (ch == LF)
				{
					te_state = body_end;
					break;
				}
				else
				{
					setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 7"); // other exception?
				}

			case body_end_cr:
				if (ch == LF)
				{
					// body_parsing_done = 1;
					// body_read = 1;
					body_unchunked = 1;
					break;
				}
				else
				{
					setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 8"); // other exception?
				}

			case body_end:
				if (ch == CR)
				{
					te_state = body_end_cr;
					break;
				}
				else if (ch == LF || !trailer_exists)
				{
					// body_parsing_done = 1;
					// body_read = 1;
					body_unchunked = 1;
					break;
				}
				else
				{
					setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 9"); // other exception?
				}
		}
	}

	// The chunked coding does not define any parameters. Their presence SHOULD be treated as an error. --> what is meant by that?


	// A server that receives a request message with a transfer coding it does not understand SHOULD respond with 501 (Not Implemented)
	// This is why Transfer-Encoding is defined as overriding Content-Length, as opposed to them being mutually incompatible.
	// A server MAY reject a request that contains both Content-Length and Transfer-Encoding or process such a request in accordance with the 
	// Transfer-Encoding alone. Regardless, the server MUST close the connection after responding to such a request to avoid the potential attacks.

	// If a valid Content-Length header field is present without Transfer-Encoding, its decimal value defines the expected message body length in octets. 
	//If the sender closes the connection or the recipient times out before the indicated number of octets are received, the recipient MUST 
	// consider the message to be incomplete and close the connection.

	// A recipient MUST ignore unrecognized chunk extensions. A server ought to limit the total length of chunk extensions received in a request 
	// to an amount reasonable for the services provided, in the same way that it applies length limitations and timeouts for other parts of a 
	// message, and generate an appropriate 4xx (Client Error) response if that amount is exceeded
}
















// std::cout << "identified method: " << request_header.getMethod() << '\n';
// std::cout << "identified path: " << request_header.getPath() << '\n';
// std::cout << "identified query: " << request_header.getQuery() << '\n';
// std::cout << "identified version: " << request_header.getHttpVersion() << '\n';

// for testing if correct configuration info reaches RequestHandler
	// for (std::vector<t_server_config>::iterator it = server_config.begin(); it != server_config.end(); it++)
	// {
	// 	std::cout << "port and server name: " << it->port << " " << it->serverName << std::endl;
	// 	for (std::vector<t_location_config>::iterator it2 = it->locations.begin(); it2 != it->locations.end(); it2++)
	// 	{
	// 		std::cout << "location: " << it2->path << std::endl;
	// 	}
	// }



	// The presence of a message body in a request is signaled by a Content-Length or Transfer-Encoding header field. Request message framing is independent of method semantics.
	// GET requests can have a body but that has no semantic meaning --> so no need to check --> still need to recv the whole body before responding?
	
	// if we expect a body (only if POST?) // parse the body based on whether the request is a GET, POST or DELETE request? --> create specific objects for those requests?
		// does the transmission format play a role here?
		// if we read max number of bytes we continue receiving bytes and always append the incoming data to a stringfile
		// for that we need to know an offset (where the header ends)
		// receive chunks?
		// for each iteration: each call of this event, you will add an oneshot event for the TIMEOUT event (EVFILT_TIMER), --> see slack bookmark

	// notes
		// video: 2h mark --> set stringstream flags
		// use uint8_t or unsigned char for storing the incoming data



