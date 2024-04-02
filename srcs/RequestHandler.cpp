
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

RequestHandler::RequestHandler(int fd, std::vector<t_server_config> server_config)
	: request_header(*this), request_body(*this)
{
	this->server_config = server_config;
	connection_fd = fd;
	status = 200;
	selected_location = 0;
	selected_server = 0;
	bytes_read = 0;
	response_ready = 0;

	request_length = 0;
	body_read = 0;
	body_beginning = 0;
	body_length = 0;

	buf_pos = -1;
	
	response = NULL;

	raw_buf.setf(std::ios::app | std::ios::binary);
	memset(&buf, 0, sizeof(buf));
}

RequestHandler::RequestHandler(/* args */)
	: request_header(*this), request_body(*this)
{
}

RequestHandler::~RequestHandler()
{
}

// RequestHandler::RequestHandler(const RequestHandler& src)
// 	: request_header(src.request_header)
// {
// }

RequestHandler& RequestHandler::operator=(const RequestHandler& src)
{
	if (this != &src)
	{
		// request_header = src.request_header;
		server_config = src.server_config;
		status = src.status;
		selected_location = src.selected_location;
		selected_server = src.selected_server;
		connection_fd = src.connection_fd;
		bytes_read = src.bytes_read;
		response_ready = src.response_ready;
		request_length = src.request_length;
		body_read = src.body_read;
		body_beginning = src.body_beginning;
		body_length = src.body_length;
		buf_pos = src.buf_pos;
		response = src.response;
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

const RequestHeader&	RequestHandler::getHeaderInfo()
{
	return (request_header);
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
	send(connection_fd, resp.c_str(), resp.length(), 0); 
	// check for errors when calling send
}

// read request handler
void	RequestHandler::processRequest()
{
	// for testing if correct configuration info reaches RequestHandler
	// for (std::vector<t_server_config>::iterator it = server_config.begin(); it != server_config.end(); it++)
	// {
	// 	std::cout << "port and server name: " << it->port << " " << it->serverName << std::endl;
	// 	for (std::vector<t_location_config>::iterator it2 = it->locations.begin(); it2 != it->locations.end(); it2++)
	// 	{
	// 		std::cout << "location: " << it2->path << std::endl;
	// 	}
	// }

	//how to handle cases in which the header is not recv in one go? (do those cases exist?)
	bytes_read = recv(connection_fd, buf, sizeof(buf), 0);
	if (bytes_read == -1)
		throw CustomException("Failed when processing read request\n");
	if (bytes_read == 0)
		return ; // also throw exception. Need to check the exception exactly // also close connection?
	// close fd in case bytes_read == 0 ???
	
	buf[bytes_read] = '\0'; // correct? BUFFER_SIZE + 1?
	request_length += bytes_read;

	printf("read %i bytes\n", bytes_read);
	printf("buf content: %s\n", buf);

	try
	{
		// check if headers have already been parsed
		if (!request_header.getHeaderStatus())
		{
			std::cout << "here" << std::endl;
			request_header.parseHeader();
			// How do the header fields in the request affect the response?
		}
		//for testing: print received headers
		printf("\nheaders\n");
		std::map<std::string, std::string> headers = request_header.getHeaderFields();
		for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		{
			std::cout << "key: " << it->first << " ";
			std::cout << "value: " << it->second << std::endl;
		}

		std::cout << "identified method: " << request_header.getMethod() << '\n';
		std::cout << "identified path: " << request_header.getPath() << '\n';
		std::cout << "identified query: " << request_header.getQuery() << '\n';
		std::cout << "identified version: " << request_header.getHttpVersion() << '\n';

		if (request_header.getHeaderExpectedStatus()) // this is relevant for POST only, should this be done in another place? (e.g. POST request class)
		{
			// check value of expect field?
			// check content-length field before accepting?
			// create response signalling the client that the body can be send
			// make reponse
		}
		// if body is expected
		if (request_header.getBodyStatus())
		{
			request_body.readBody();
			//here we need to account for max-body_size specified in config file
			// if chunked
				//store body chunks in file (already store in the appropriate object)
			// if not chunked
				// store body in stringstream or vector (already store in the appropriate object)
			// if end of body has not been reached
				// return to continue receiving
		}
		// if no body is expected OR end of body has been reached
		if (!request_header.getBodyStatus() || body_read)
		{
			std::cout << "body content: " << request_body.body << std::endl;
			// try/catch block?
			response = prepareResponse();
			response->createResponse();
			// response = request->createResponse();
			// set Response to be ready
		}
		response_ready = 1;

	}
	catch(const std::exception& e)
	{
		response = new ERRORResponse(*this); // need to free this somewhere
		response->createResponse();
		response_ready = 1;
		std::cerr << e.what() << '\n';
	}

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
}


AResponse* RequestHandler::prepareResponse()
{
	// find server block if there are multiple that match (this applies to all request types)
	if (server_config.size() > 1)
		findServerBlock();
	
	// find location block within server block if multiple exist (this applies to all request types; for GET requests there might be an internal redirect happening later on)
	if (server_config[selected_server].locations.size() > 1)
		findLocationBlock();

	if (request_header.getMethod() == "GET")
		return (new GETResponse(*this)); // need to free this somewhere
	else if (request_header.getMethod() == "DELETE")
		return (new DELETEResponse(*this)); // need to free this somewhere
	else if (request_header.getMethod() == "POST")
		throw CustomException("test");///
	return (NULL);
}


void	RequestHandler::findLocationBlock() // double check if this is entirely correct approach
{
	std::vector<std::string> uri_path_items;
	if (response == NULL || response->getRedirectedPath().empty())
		uri_path_items = splitPath(request_header.getPath(), '/');
	else
	{
		std::string temp = "/" + response->getRedirectedPath();
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





