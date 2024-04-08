
#include "RequestBody.h"
#include "RequestHandler.h"

RequestBody::RequestBody(/* args */)
	: handler(*new RequestHandler())
{
	body_parsing_done = 0;
	chunk_length = 0;
	trailer_exists = 0;
	te_state = body_start;
	mp_state = mp_start;
	content_dispo_state = begin;
	content_type_state = begin2;
	content_type = unknown;
	// outFile.setf(std::ios::app | std::ios::binary);
	// temp.setf(std::ios::app | std::ios::binary);
}

RequestBody::RequestBody(RequestHandler& src)
	: handler(src)
{
	body_parsing_done = 0;
	chunk_length = 0;
	trailer_exists = 0;
	te_state = body_start;
	mp_state = mp_start;
	content_dispo_state = begin;
	content_type_state = begin2;
	content_type = unknown;
	// outFile.setf(std::ios::app | std::ios::binary);
	// temp.setf(std::ios::app | std::ios::binary);
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


void	RequestBody::checkBoundaryID()
{
	handler.buf_pos++; // why necessary?

	for (std::string::iterator it = boundary.begin(); it != boundary.end(); it++) // also check handler.buf_pos < handler.getBytesRead()
	{
		handler.buf_pos++;
		if (*it != handler.buf[handler.buf_pos])
		{
			handler.setStatus(400); // what is the correct error code?
			throw CustomException("Issue with boundary ID 1"); // other exception?
		}
	}
	handler.buf_pos++;
	if (handler.buf[handler.buf_pos] == CR)
	{
		handler.buf_pos++;
		if (handler.buf[handler.buf_pos] != LF)
		{
			handler.setStatus(400); // what is the correct error code?
			throw CustomException("Issue with boundary ID 2"); // other exception?
		}
	}
	else if (handler.buf[handler.buf_pos] != LF)
	{
		handler.setStatus(400); // what is the correct error code?
		throw CustomException("Issue with boundary ID 3"); // other exception?
	}
	mp_state = mp_content_dispo;
}


void	RequestBody::saveContentDispo()
{
	std::string temp_value;
	std::string temp_key;

	// integrate check for CR and LF into Switch statement
	while (handler.buf_pos < handler.getBytesRead() && (handler.buf[handler.buf_pos] != CR && handler.buf[handler.buf_pos] != LF))
	{
		unsigned char ch = handler.buf[handler.buf_pos];

		switch (content_dispo_state)
		{
		case begin:
			if (ch == ':')
				content_dispo_state = type;
			break;
		
		case type:
			if (ch == ';')
			{
				content_disposition.insert(std::pair<std::string, std::string>("type", temp_value));
				temp_value.erase();
				content_dispo_state = var_key;
				break;
			}
			else if (ch == SP)
				break;
			else
			{
				temp_value.append(1, ch);
				break;
			}

		case var_key:
			if (ch == '=')
			{
				content_dispo_state = var_value;
				break;
			}
			else if (ch == SP)
				break;
			else
			{
				temp_key.append(1, ch);
				break;
			}

		case var_value:
			if (ch == ';')
			{
				content_disposition.insert(std::pair<std::string, std::string>(temp_key, temp_value));
				temp_key.erase();
				temp_value.erase();
				content_dispo_state = var_key;
				break;
			}
			else if (ch == '"')
				break;
			else
			{
				temp_value.append(1, ch);
				break;
			}
		}
		handler.buf_pos++;
	}
	if (handler.buf_pos >= handler.getBytesRead())
		return ;
	else if (handler.buf[handler.buf_pos] == CR)
	{
		handler.buf_pos++;
		if (handler.buf[handler.buf_pos] != LF)
		{
			handler.setStatus(400); // what is the correct error code?
			throw CustomException("Issue with content dispo 1"); // other exception?
		}
	}
	else if (handler.buf[handler.buf_pos] != LF)
	{
		handler.setStatus(400); // what is the correct error code?
		throw CustomException("Issue with content dispo 2"); // other exception?
	}
	content_disposition.insert(std::pair<std::string, std::string>(temp_key, temp_value));
	temp_key.erase();
	temp_value.erase();
	mp_state = mp_content_type;
}

void	RequestBody::saveContentType()
{
	while (handler.buf_pos < handler.getBytesRead() && (handler.buf[handler.buf_pos] != CR && handler.buf[handler.buf_pos] != LF))
	{
		unsigned char ch = handler.buf[handler.buf_pos];

		switch (content_type_state)
		{
			case begin2:
				if (ch == ':')
				{
					content_type_state = type_name;
					break;
				}
				else
					break;
			
			case type_name:
				if (ch == SP)
					break;
				else
				{
					multipart_content_type.append(1, ch);
					break;
				}
		}
		handler.buf_pos++;
	}
	if (handler.buf_pos >= handler.getBytesRead())
	{
		printf("stuck in content type?\n");
		return ;
	}
	else if (handler.buf[handler.buf_pos] == CR)
	{
		handler.buf_pos++;
		if (handler.buf[handler.buf_pos] != LF)
		{
			handler.setStatus(400); // what is the correct error code?
			throw CustomException("Issue with content dispo"); // other exception?
		}
	}
	else if (handler.buf[handler.buf_pos] != LF)
	{
		handler.setStatus(400); // what is the correct error code?
		throw CustomException("Issue with content dispo"); // other exception?
	}
	mp_state = mp_empty_line;
}

void	RequestBody::storeContent()
{
	handler.buf_pos--;
	temp.open(content_disposition["filename"], std::ios::app | std::ios::binary); 
	while (handler.buf_pos++ < handler.getBytesRead() - 1) // not entirely sure why to add "-1" here, though ...
	{
		unsigned char ch = handler.buf[handler.buf_pos];

		if (ch == CR)
		{
			if (handler.buf[handler.buf_pos + 1] == LF && handler.buf[handler.buf_pos + 2] == '-' && handler.buf[handler.buf_pos + 3] == '-')
			{
				mp_state = mp_boundary_end;
				break;
			}
		}
		else if (ch == LF)
		{
			if (handler.buf[handler.buf_pos + 1] == '-' && handler.buf[handler.buf_pos + 2] == '-')
			{
				mp_state = mp_boundary_end;
				break;
			}
		}
		temp.write(reinterpret_cast<const char*>(&handler.buf[handler.buf_pos]), 1); // how to read larger part at a time instead of byte per byte
	}
	temp.close();
}

void	RequestBody::parsePlainBody()
{
	while (!body_parsing_done && handler.buf_pos++ < handler.getBytesRead() - 1) // not entirely sure why to add "-1" here, though ...
	{
		unsigned char ch = handler.buf[handler.buf_pos];

		switch (mp_state)
		{
			case mp_start:
				if (ch == '-')
					mp_state = mp_boundary_id;
				else
				{
					handler.setStatus(400); // what is the correct error code?
					throw CustomException("Bad request 20"); // other exception?
				}

			case mp_boundary_id:
				checkBoundaryID();
				break;
			
			case mp_content_dispo:
				saveContentDispo();
				for (std::map<std::string, std::string>::iterator it = content_disposition.begin(); it != content_disposition.end(); it++)
				{
					std::cout << "key: " << it->first << " value: " << it->second << std::endl;
				}
				break;

			case mp_content_type:
				if (ch == CR)
					break;
				else if (ch == LF)
				{
					mp_state = mp_content;
					break;
				}
				else if (ch == 'C')
				{
					saveContentType();
					std::cout << "content type: " << multipart_content_type << std::endl;
					break;
				}
				else
				{
					handler.setStatus(400); // what is the correct error code?
					throw CustomException("Issue with content type"); // other exception?
				}

			case mp_empty_line:
				if (ch == CR)
					break;
				else if (ch == LF)
				{
					mp_state = mp_content;
					break;
				}

			case mp_content:
				storeContent();
				break;
				
			case mp_boundary_end:
				handler.body_read = 1;
				body_parsing_done = 1;
				break;
		}
	}
}

void	RequestBody::checkContentType()
{
	std::string value = handler.getHeaderInfo().getHeaderFields()["content-type"];
	std::string type;
	
	// identify content type
	std::size_t delim_pos = value.find(';');
	if (delim_pos != std::string::npos)
		type = value.substr(0, delim_pos);
	else
		type = value;
	
	if (type == "multipart/form-data")
		content_type = multipart_form;
	
	// identify boundary
	if (content_type == multipart_form)
	{
		std::size_t boundary_value_pos = value.find("=");
		if (boundary_value_pos != std::string::npos)
			boundary = value.substr(boundary_value_pos + 1);
		else
		{
			handler.setStatus(400); // what is the correct error code?
			throw CustomException("Could not identify boundary"); // other exception?
		}
	}
}

void	RequestBody::readBody()
{
	checkContentType();
	if (handler.getHeaderInfo().getTEStatus())
		parseChunkedBody(); // inside content type needs to be checked
	else
		parsePlainBody(); // inside content type needs to be checked

	// how do other types than multiform look like?
}
