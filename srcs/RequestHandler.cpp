
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

RequestHandler::RequestHandler(int fd, std::vector<t_server_config> server_config)
	: header(*this)
{
	this->server_config = server_config;
	connection_fd = fd;
	status = 200;
	selected_location = 0;
	selected_server = 0;
	bytes_read = 0;
	response_ready = 0;

	body_parsing_done = 0;
	chunk_length = 0;
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
	: header(*this)
{
}

RequestHandler::~RequestHandler()
{
}

// RequestHandler::RequestHandler(const RequestHandler& src)
// 	: header(src.header)
// {
// }

RequestHandler& RequestHandler::operator=(const RequestHandler& src)
{
	if (this != &src)
	{
		// header = src.header;
		server_config = src.server_config;
		status = src.status;
		selected_location = src.selected_location;
		selected_server = src.selected_server;
		connection_fd = src.connection_fd;
		bytes_read = src.bytes_read;
		response_ready = src.response_ready;
		body_parsing_done = src.body_parsing_done;
		chunk_length = src.chunk_length;
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
	return (header);
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

	try
	{
		// check if headers have already been parsed
		if (!header.getHeaderStatus())
		{
			header.parseHeader();
			// How do the header fields in the request affect the response?
		}
		//for testing: print received headers
		printf("\nheaders\n");
		std::map<std::string, std::string> headers = header.getHeaderFields();
		for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		{
			std::cout << "key: " << it->first << " ";
			std::cout << "value: " << it->second << std::endl;
		}

		std::cout << "identified method: " << header.getMethod() << '\n';
		std::cout << "identified path: " << header.getPath() << '\n';
		std::cout << "identified query: " << header.getQuery() << '\n';
		std::cout << "identified version: " << header.getHttpVersion() << '\n';

		if (header.getHeaderExpectedStatus()) // this is relevant for POST only, should this be done in another place? (e.g. POST request class)
		{
			// check value of expect field?
			// check content-length field before accepting?
			// create response signalling the client that the body can be send
			// make reponse
		}
		// if body is expected
		if (header.getBodyStatus())
		{
			//here we need to account for max-body_size specified in config file
			// if chunked
				//store body chunks in file (already store in the appropriate object)
			// if not chunked
				// store body in stringstream or vector (already store in the appropriate object)
			// if end of body has not been reached
				// return to continue receiving
		}
		// if no body is expected OR end of body has been reached
		if (!header.getBodyStatus() || body_read)
		{
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
		// if there are any endoded characters in the header, they need to be decoded
			// Some characters are utilized by URLs for special use in defining their syntax. When these characters are not used in their special role inside a URL, they must be encoded.
			// characters such as {} are getting encoded by the client(?) and being transmitted e.g. with %7B%7D
}


AResponse* RequestHandler::prepareResponse()
{
	// find server block if there are multiple that match (this applies to all request types)
	if (server_config.size() > 1)
		findServerBlock();
	
	// find location block within server block if multiple exist (this applies to all request types; for GET requests there might be an internal redirect happening later on)
	if (server_config[selected_server].locations.size() > 1)
		findLocationBlock();

	if (header.getMethod() == "GET")
		return (new GETResponse(*this)); // need to free this somewhere
	else if (header.getMethod() == "DELETE")
		return (new DELETEResponse(*this)); // need to free this somewhere
	else if (header.getMethod() == "POST")
		;///
	return (NULL);
}


void	RequestHandler::findLocationBlock() // double check if this is entirely correct approach
{
	std::vector<std::string> uri_path_items;
	if (response == NULL || response->getRedirectedPath().empty())
		uri_path_items = splitPath(header.getPath(), '/');
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
		if (server_config[i].serverName == header.getHeaderFields()["host"])
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




void	RequestHandler::parseEncodedBody()
{
	// check somewhere that when the transfer encoding contains something different than "chunked" to return an error

	te_state = body_start;

	while (!body_parsing_done && buf_pos++ < bytes_read)
	{
		u_char ch = buf[buf_pos];

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
					status = 400; // what is the correct error code?
					throw CustomException("Bad request 1"); // other exception?
				}

			case chunk_size:
				std::cout << "len: " << chunk_length << std::endl;
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
				else if (chunk_length == 0)
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
					else if (ch == SP)
					{
						te_state = chunk_extension; // are there more seperators? // seperate state for last extension?
						break;
					}
					else
					{
						status = 400; // what is the correct error code?
						throw CustomException("Bad request 2"); // other exception?
					}
				}
				else if (ch == CR)
				{
					te_state = chunk_size_cr;
					break;
				}
				else if (ch == LF)
				{
					te_state = chunk_data;
					break;
				}
				else if (ch == SP) // are there more seperators?
				{
					te_state = chunk_extension;
					break;
				}
				else
				{
					status = 400; // what is the correct error code?
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
					status = 400; // what is the correct error code?
					throw CustomException("Bad request 4"); // other exception?
				}

			case chunk_extension:
				// read extension
				// go to chunk data reading or chung_size_cr
				break;

			case chunk_data:
				for (int i = 0; i < chunk_length; i++)
				{
					ch = buf[buf_pos];
					if (ch == CR)
					{
						te_state = chunk_data_cr;
						break;
					}
					// else if (ch == LF) // remove here?
					// {
					// 	te_state = body_start;
					// 	break;
					// }
					else
					{
						body << ch;
						buf_pos++;
					}
				}
				ch = buf[buf_pos];
				// calc content-length
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
				else
				{
					status = 400; // what is the correct error code?
					throw CustomException("Bad request 5"); // other exception?
				}

			
			case chunk_data_cr:
				if (ch == LF)
				{
					te_state = body_start;
					break;
				}
				else
				{
					status = 400; // what is the correct error code?
					throw CustomException("Bad request 6"); // other exception?
				}
			
			// is the existence of trailers indicated in the headers
			case chunk_trailer:
				body_parsing_done = 1;
				te_state = body_end;
				break;
				// check if trailer is existing
				// read trailer

			case body_end:
				break;
				// termination

		}		

	}

	buf_pos++;

		// check chunk-size (first part of body) and translate from hex to integer; followed by CRLF if chunk extension is not provided
			// if chunk-size is 0 and followd by CRLF, the end of the transmission has been reached

		// check for chunk extension, this is followed by CRLF
			// A recipient MUST ignore unrecognized chunk extensions // how to recognize an invalid one?
			// Where to store that extension?

		// run through the chunk data using chunk-size as a delimiter
			// add content to stream until CRLF is reached
			// count data length
			// check again for chunk size and chunk extension

		// if end of data transimission (chunk size 0), check for trailer (finally terminated by empty line --> CRLFCRLF??)
			// store somewhere
			// what info does trailer entail?

	// empty line termination --> always? or only when chunk size == 0?
	
	// set content-length in headers to counted data length --> for what purpose?
	
	


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

	// use ostringstream to store body?
	
	// chunked-body   = *chunk
                   //last-chunk
                   //trailer-section
                   //CRLF
}

