
#include "RequestBody.h"
#include "RequestHandler.h"

RequestBody::RequestBody(/* args */)
	: handler(*new RequestHandler())
{
	body_parsing_done = 0;
	chunk_length = 0;
}

RequestBody::RequestBody(RequestHandler& src)
	: handler(src)
{
	body_parsing_done = 0;
}

RequestBody::~RequestBody()
{
}

void	RequestBody::parseChunkedBody()
{
	te_state = body_start;

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
				else if (ch == LF)
				{
					te_state = chunk_data;
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
				// trailer fields can be useful for supplying message integrity checks, digital signatures, delivery metrics, or post-processing status information
				// probably can just discard this section --> how to identify end?
				body_parsing_done = 1; // for testing
				handler.body_read = 1; // for testing
				te_state = body_end;
				break;
				// check if trailer is existing
				// read trailer

			case body_end:
				body_parsing_done = 1;
				handler.body_read = 1;
				break;
				// termination

		}		

	}

	handler.buf_pos++;

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

void	RequestBody::readBody()
{
	// if (handler.getHeaderInfo().getTEStatus())
	parseChunkedBody();
	// if chunked
		//store body chunks in file (already store in the appropriate object)
	// if not chunked
		// store body in stringstream or vector (already store in the appropriate object)
}
