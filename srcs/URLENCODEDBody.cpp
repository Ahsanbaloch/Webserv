
#include "URLENCODEDBody.h"

/////////// CONSTRUCTORS & DESTRUCTORS ///////////

URLENCODEDBody::URLENCODEDBody()
	: ARequestBody()
{
	encoded_key = 0;
	encoded_value = 0;
	body_bytes_consumed = 0;
	body_state = key;
}

URLENCODEDBody::URLENCODEDBody(RequestHandler& src)
	: ARequestBody(src)
{
	encoded_key = 0;
	encoded_value = 0;
	body_bytes_consumed = 0;
	body_state = key;
}

URLENCODEDBody::~URLENCODEDBody()
{
}

URLENCODEDBody::URLENCODEDBody(const URLENCODEDBody& src)
	: ARequestBody(src)
{
	// copy input filestream --> how?
	encoded_key = src.encoded_key;
	encoded_value = src.encoded_value;
	body_bytes_consumed = src.body_bytes_consumed;
	database = src.database;
	temp_value = src.temp_value;
	temp_key = src.temp_key;
	body_state = src.body_state;
}

URLENCODEDBody& URLENCODEDBody::operator=(const URLENCODEDBody& src)
{
	if (this != &src)
	{
		ARequestBody::operator=(src);
		encoded_key = src.encoded_key;
		encoded_value = src.encoded_value;
		body_bytes_consumed = src.body_bytes_consumed;
		database = src.database;
		temp_value = src.temp_value;
		temp_key = src.temp_key;
		body_state = src.body_state;
	}
	return (*this);
}


/////////// HELPER METHODS ///////////

void	URLENCODEDBody::storeInDatabase()
{
	if (encoded_key)
	{
		decode(temp_key);
		encoded_key = 0;
	}
	if (encoded_value)
	{
		decode(temp_value);
		encoded_value = 0;
	}
	database.insert(std::pair<std::string, std::string>(temp_key, temp_value));
	temp_key.erase();
	temp_value.erase();
	body_state = key;
}

void	URLENCODEDBody::parseBody(char ch)
{
	switch (body_state)
	{
		case key:
			if (ch == CR || ch == LF)
			{
				handler.setStatus(400); // what is the correct error code?
				throw CustomException("Bad request"); // other exception?
			}
			else if (ch == '=')
			{
				body_state = value;
				break;
			}
			else if (ch == '%')
			{
				encoded_key = 1;
				temp_key.append(1, ch);
				break;
			}
			else
			{
				temp_key.append(1, ch);
				break;
			}

		case value:
			if (ch == CR || ch == LF)
			{
				handler.setStatus(400); // what is the correct error code?
				throw CustomException("Bad request"); // other exception?
			}
			else if (ch == '&')
				body_state = end_pair;
			else if (ch == '%')
			{
				encoded_value = 1;
				temp_value.append(1, ch);
				break;
			}
			else
			{
				temp_value.append(1, ch);
				break;
			}

		case end_pair:
			storeInDatabase();
	}
}

void	URLENCODEDBody::decode(std::string& sequence)
{
	for (std::string::iterator it = sequence.begin(); it != sequence.end(); it++)  // allowed values #01 - #FF (although ASCII only goes till #7F/7E)
	{
		if (*it == '%')
		{
			sequence.erase(it);
			unsigned int decoded_char;
			std::stringstream ss;
			if ((*it >= '0' && *it <= '9') || (*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z'))
			{
				ss << std::hex << *it;
				sequence.erase(it);
			}
			else
			{
				handler.setStatus(400);
				throw CustomException("decoding error");
			}
			if ((*it >= '0' && *it <= '9') || (*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z'))
			{
				ss << std::hex << *it;
				ss >> decoded_char;
				*it = decoded_char;
			}
			else
			{
				handler.setStatus(400);
				throw CustomException("decoding error");
			}
		}
		if (*it == '+')
			*it = ' ';
	}
}


/////////// MAIN METHOD ///////////

void	URLENCODEDBody::readBody()
{
	if (handler.getHeaderInfo().getTEStatus())
	{
		unchunkBody();
		if (body_read)
		{
			input.open(filename, std::ios::ate);
			std::streamsize file_size = input.tellg();
			input.seekg(0, std::ios::beg);
			char ch;
			for (int i = 0; i < file_size; i++)
			{
				input.read(&ch, 1);
				parseBody(ch);
			}
			storeInDatabase();
			input.close();
			remove(filename.c_str());
		}
	}
	else
	{
		while (body_bytes_consumed < handler.getHeaderInfo().getBodyLength() && handler.buf_pos < handler.getBytesRead())
		{
			handler.buf_pos++;
			body_bytes_consumed++;
			unsigned char ch = handler.buf[handler.buf_pos];
			parseBody(ch);
		}
		if (body_bytes_consumed >= handler.getHeaderInfo().getBodyLength())
		{
			storeInDatabase();
			body_read = 1;
		}

		//testing
		for (std::map<std::string, std::string>::iterator it = database.begin(); it != database.end(); it++)
		{
			std::cout << "key: " << it->first << " value: " << it->second << std::endl;
		}
	}
}
