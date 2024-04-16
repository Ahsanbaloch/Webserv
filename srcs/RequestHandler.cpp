
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

RequestHandler::RequestHandler()
	: request_header(*this)
{
}

RequestHandler::RequestHandler(int fd, std::vector<t_server_config> server_config)
	: request_header(*this)
{
	this->server_config = server_config;
	connection_fd = fd;
	status = 200;
	selected_location = 0;
	selected_server = 0;
	bytes_read = 0;
	response_ready = 0;
	request_length = 0;

	buf_pos = -1;
	
	response = NULL;
	request_body = NULL;
	memset(&buf, 0, sizeof(buf));
}

RequestHandler::~RequestHandler()
{
	delete response;
	delete request_body;
}

RequestHandler::RequestHandler(const RequestHandler& src)
	: request_header(src.request_header)
{
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
		request_body = src.request_body;
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

t_server_config	RequestHandler::getSelectedServer() const
{
	return (server_config[selected_server]);
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
			request_header.parseHeader();
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
			// if body is expected, read the body
			if (request_header.getBodyStatus())
			{
				if (request_body == NULL)
					request_body = checkContentType(); // needs to be deleted/freed somewhere
				request_body->readBody();
			}
			// if no body is expected OR end of body has been reached
			if (!request_header.getBodyStatus() || (request_body != NULL && request_body->getBodyProcessed()))
			{
				// std::cout << "body content: " << request_body.body << std::endl;
				response = prepareResponse(); // how to handle errors in here?
				response->createResponse(); // how to handle errors in here?
				response_ready = 1;
			}
		}
	}
	catch(const std::exception& e)
	{
		response = new ERRORResponse(*this); // need to free this somewhere
		response->createResponse();
		response_ready = 1;
		std::cerr << e.what() << '\n';
	}
}


AResponse* RequestHandler::prepareResponse()
{
	// find server block if there are multiple that match (this applies to all request types)
	if (server_config.size() > 1)
		findServerBlock();
	
	// find location block within server block if multiple exist (this applies to all request types; for GET requests there might be an internal redirect happening later on)
	if (server_config[selected_server].locations.size() > 1)
		findLocationBlock();
	//CGi Extension Check to be done here
	if (request_header.getMethod() == "GET")
		return (new GETResponse(*this)); // need to free this somewhere
	else if (request_header.getMethod() == "DELETE")
		return (new DELETEResponse(*this)); // need to free this somewhere
	else if (request_header.getMethod() == "POST")
		return (new POSTResponse(*this));
	return (NULL);
}


ARequestBody*	RequestHandler::checkContentType()
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
		return (new MULTIPARTBody(*this));
	}
	else if (type == "application/x-www-form-urlencoded")
	{
		content_type = urlencoded;
		return (new URLENCODEDBody(*this));
	}
	else
	{
		// or throwException if type is not supported?
		content_type = text_plain;
		return (new PLAINBody(*this));
	}
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

