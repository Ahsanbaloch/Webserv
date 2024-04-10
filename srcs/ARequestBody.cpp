
#include "ARequestBody.h"
#include "RequestHandler.h"

ARequestBody::ARequestBody(/* args */)
	: handler(*new RequestHandler())
{
	body_parsing_done = 0;
	chunk_length = 0;
	trailer_exists = 0;
	total_chunk_size = 0;
	// bytes_written = 0;
	te_state = body_start;
	// content_type = unknown;
	// outFile.setf(std::ios::app | std::ios::binary);
	// temp.setf(std::ios::app | std::ios::binary);
}

ARequestBody::ARequestBody(RequestHandler& src)
	: handler(src)
{
	body_parsing_done = 0;
	chunk_length = 0;
	trailer_exists = 0;
	total_chunk_size = 0;
	// bytes_written = 0;
	te_state = body_start;
	// content_type = unknown;
	// outFile.setf(std::ios::app | std::ios::binary);
	// temp.setf(std::ios::app | std::ios::binary);
}

ARequestBody::~ARequestBody()
{
}

void	ARequestBody::storeChunkedData()
{
	// if content-type plain text use file name from header path (also check for existence, if not existing, create file)
	if (handler.content_type == handler.text_plain)
	{
		// header should probably also provide file name? --> needed somewhere else? (in GET?)
		// should be done somewhere else so that this is check is not done over and over again.
		filename = "testing.txt";
		
	}
	else if (handler.content_type == handler.multipart_form || handler.content_type == handler.urlencoded)
		filename = "temp.bin";

	temp2.open(filename, std::ios::app | std::ios::binary);
	temp2.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), chunk_length);
	handler.buf_pos += chunk_length;
	temp2.close();

}

void	ARequestBody::unchunkBody()
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
				storeChunkedData();
				ch = handler.buf[handler.buf_pos];
				total_chunk_size += chunk_length;
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
