
#include "RequestHandler.h"

RequestHandler::RequestHandler(int fd)
{
	connection_fd = fd;
	error = 0;
	buf_pos = -1;
	// rl_parsing_done = 0;
	// headers_parsing_done = 0;
	body_parsing_done = 0;
	// transfer_encoding_exists = 0;
	// content_length_exists = 0;
	// expect_exists = 0;
	// host_exists = 0;
	chunk_length = 0;
	// body_length = 0;
	// body_beginning = 0;
	// header_complete = 0;
	raw_buf.setf(std::ios::app | std::ios::binary);
	// method = ""; // does this reset the string?
	// path = ""; // does this reset the string?
	// query = ""; // does this reset the string?
	// version = ""; // does this reset the string?
	memset(&buf, 0, sizeof(buf));
}

RequestHandler::RequestHandler(/* args */)
{
}

RequestHandler::~RequestHandler()
{
}

// read request handler
void	RequestHandler::handleRequest()
{
	bytes_read = recv(connection_fd, buf, sizeof(buf), 0);
	if (bytes_read == -1)
		throw CustomException("Failed when processing read request\n");
	if (bytes_read == 0)
		return ; // also throw exception. Need to check the exception exactly // also close connection?
	
	buf[bytes_read] = '\0'; // correct? BUFFER_SIZE + 1?
	request_length += bytes_read;

	// check if headers have already been parsed
	if (!header.header_complete)
	{
		try
		{
			// what about folding lines?
			header.parseRequestLine(buf, &buf_pos, bytes_read);
			header.parseHeaderFields(buf, &buf_pos, bytes_read); // check if it still works if no header is sent
			// check for body existance
			// check if requested resource exists?
			// decode URL/Query if necessary
			// construct full URI?
			// create object for POST/DELETE/GET (copy all relevant data?)
		}
		catch(const std::exception& e)
		{
			// terminate connection and remove file descriptor from the event queue
			std::cerr << e.what() << '\n';
		}
	}
	// if immediate response is expected to receive body
		// make reponse
	// if body is expected
		// if chunked
			//store body chunks in file (already store in the appropriate object)
		// if not chunked
			// store body in stringstream or vector (already store in the appropriate object)
		// if end of body has not been reached
			// return to continue receiving
	// if no body is expected OR end of body has been reached
		// process request (based on the object type that has been created --> through base pointer?)



	// The presence of a message body in a request is signaled by a Content-Length or Transfer-Encoding header field. Request message framing is independent of method semantics.

	// interpret parsed input to decide what to do next
		// construct targetURI? --> info from config file?
		// --> split in GET / POST / DELETE ??
		// GET requests can have a body but that has no semantic meaning --> so no need to check --> still need to recv the whole body before responding?
		// --> also check if there is an expected immediate response, e.g. 100-continue
		// if there are any encoded chars, decode them here? --> relevant for url/query
		// construct the full URI here?
	
		// check if header includes Expect: 100-continue
		// determine position where the header ends
	
	
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

	
	printf("\nheaders\n");
	for (std::map<std::string, std::string>::iterator it = header.headers.begin(); it != header.headers.end(); it++)
	{
		std::cout << "key: " << it->first << " ";
		std::cout << "value: " << it->second << std::endl;
	}

	
	// if (transfer_encoding_exists && !content_length_exists) // creates issue with postman
	// 	parseEncodedBody();
	// if (transfer_encoding_exists)
	// 	parseEncodedBody();
	// else if (content_length_exists && !transfer_encoding_exists)
	// 	parseContentBody();
	// else
	// {
	// 	error = 400;
	// 	throw CustomException("Bad request");
	// 	// A server MAY reject a request that contains a message body but not a Content-Length by responding with 411 (Length Required).
	// }
	
	std::cout << "identified method: " << header.method << '\n';
	std::cout << "identified path: " << header.path << '\n';
	std::cout << "identified query: " << header.query << '\n';
	std::cout << "identified version: " << header.version << '\n';



	printf("read %i bytes\n", bytes_read);

	// close fd in case bytes_read == 0 ???
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
					error = 400; // what is the correct error code?
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
						error = 400; // what is the correct error code?
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
					error = 400; // what is the correct error code?
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
					error = 400; // what is the correct error code?
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
					error = 400; // what is the correct error code?
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
					error = 400; // what is the correct error code?
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

