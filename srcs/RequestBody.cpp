
#include "RequestBody.h"
#include "RequestHandler.h"

RequestBody::RequestBody(/* args */)
	: handler(*new RequestHandler())
{
	body_parsing_done = 0;
	chunk_length = 0;
	trailer_exists = 0;
	te_state = body_start;
	// outFile.setf(std::ios::app | std::ios::binary);
}

RequestBody::RequestBody(RequestHandler& src)
	: handler(src)
{
	body_parsing_done = 0;
	chunk_length = 0;
	trailer_exists = 0;
	te_state = body_start;
	// outFile.setf(std::ios::app | std::ios::binary);
}

RequestBody::~RequestBody()
{
}

void	RequestBody::parseChunkedBody()
{
	while (!body_parsing_done && handler.buf_pos++ < handler.getBytesRead())
	{
		unsigned char ch = handler.buf[handler.buf_pos];

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
					handler.setStatus(400); // what is the correct error code?
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
						handler.setStatus(400); // what is the correct error code?
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
				else if (ch == ';')
				{
					te_state = chunk_extension;
					break;
				}
				else
				{
					handler.setStatus(400); // what is the correct error code?
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
					handler.setStatus(400); // what is the correct error code?
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
				for (int i = 0; i < chunk_length; i++) // probably want to store it in something else than a string already? May also depend on content-type header field
				{
					body.append(1, handler.buf[handler.buf_pos]);
					handler.buf_pos++;
				}
				ch = handler.buf[handler.buf_pos];
				// calc content-length???
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
					handler.setStatus(400); // what is the correct error code?
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
					handler.setStatus(400); // what is the correct error code?
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
					handler.setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 7"); // other exception?
				}

			case body_end_cr:
				if (ch == LF)
				{
					body_parsing_done = 1;
					handler.body_read = 1;
					break;
				}
				else
				{
					handler.setStatus(400); // what is the correct error code?
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
					body_parsing_done = 1;
					handler.body_read = 1;
					break;
				}
				else
				{
					handler.setStatus(400); // what is the correct error code?
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


// void	RequestBody::processChunkedBody()
// {


// 	// std::ofstream outFile("output.bin", std::ios::binary);
// 	// outFile.write(reinterpret_cast<char*>(handler.buf), BUFFER_SIZE - handler.body_beginning);
// 	// outFile.close();

// 	// std::ifstream inFile("output.bin", std::ios::binary);

// 	// // Get the size of the file
// 	// inFile.seekg(0, std::ios::end);
// 	// std::streamsize size = inFile.tellg();
// 	// inFile.seekg(0, std::ios::beg);

// 	// // Create a buffer to hold the data
// 	// std::vector<unsigned char> buffer(size);

// 	// // Read the data into the buffer
// 	// if (inFile.read(reinterpret_cast<char*>(buffer.data()), size))
// 	// {
// 	// 	// Parse the data
// 	// 	std::cout << "body test: " << std::endl;
// 	// 	for (long i = 0; i < size; ++i)
// 	// 	{
// 	// 		printf("%c", buffer[i]);
// 	// 	}
// 	// }
// 	// printf("\n");

// 	// inFile.close();
// }


void	RequestBody::parsePlainBody()
{
	std::ofstream	temp("temp.png", std::ios::app | std::ios::binary);
	temp.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos + 1]), handler.getBytesRead() - (handler.buf_pos + 1));
	
	// while (handler.buf_pos++ < handler.getBytesRead() && handler.body_length > 0)
	// {
	// 	body.append(1, handler.buf[handler.buf_pos]);
	// 	handler.body_length--;
	// }
	printf("Body len: %i\n", handler.body_length);
	temp.close();

	std::ifstream tempFile("temp.png", std::ios::binary);
	tempFile.seekg(0, std::ios::end);
	std::streampos fileSize = tempFile.tellg();
	tempFile.close();
	std::cout << "Size of temp file: " << fileSize << std::endl;
	printf("body len: %i\n", handler.body_length);
	temp.close();

	if (handler.body_length == fileSize)
		handler.body_read = 1;

	if (handler.body_read)
	{
		std::string line;
		std::string boundary1 = handler.getHeaderInfo().getHeaderFields()["content-type"];
		std::string boundary2 = boundary1.substr(boundary1.find('=') + 1);
		std::cout << "Boundary ident: " << boundary2 << std::endl;
		bool inBoundary = false;
		std::ifstream	temp("temp.png", std::ios::binary);
		std::ofstream	outputFile("test.png", std::ios::app | std::ios::binary);

	
		

		temp.close();
		outputFile.close();
	}

	// if (handler.body_length == 0)
	// 	handler.body_read = 1;
}

void	RequestBody::readBody()
{
	if (handler.getHeaderInfo().getTEStatus())
		parseChunkedBody(); // //store body chunks in file (already store in the appropriate object)
	else
		parsePlainBody(); // store body in stringstream or vector / could also be a file (already store in the appropriate object)
}
