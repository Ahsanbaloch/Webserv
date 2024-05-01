
#include "UploadUrlencoded.h"

/////////// CONSTRUCTORS & DESTRUCTORS ///////////

UploadUrlencoded::UploadUrlencoded()
	: AUploadModule()
{
	encoded_key = 0;
	encoded_value = 0;
	body_bytes_consumed = 0;
	body_state = key;
}

UploadUrlencoded::UploadUrlencoded(RequestHandler& src)
	: AUploadModule(src)
{
	encoded_key = 0;
	encoded_value = 0;
	body_bytes_consumed = 0;
	body_state = key;
}

UploadUrlencoded::~UploadUrlencoded()
{
}

UploadUrlencoded::UploadUrlencoded(const UploadUrlencoded& src)
	: AUploadModule(src)
{
	encoded_key = src.encoded_key;
	encoded_value = src.encoded_value;
	body_bytes_consumed = src.body_bytes_consumed;
	temp_value = src.temp_value;
	temp_key = src.temp_key;
	body_state = src.body_state;
}

UploadUrlencoded& UploadUrlencoded::operator=(const UploadUrlencoded& src)
{
	if (this != &src)
	{
		AUploadModule::operator=(src);
		encoded_key = src.encoded_key;
		encoded_value = src.encoded_value;
		body_bytes_consumed = src.body_bytes_consumed;
		temp_value = src.temp_value;
		temp_key = src.temp_key;
		body_state = src.body_state;
	}
	return (*this);
}


/////////// HELPER METHODS ///////////

void	UploadUrlencoded::storeInJSON()
{
	std::string data = convertToJSON();

	while (1)
	{
		filepath_outfile = "www/form_data/" + genRandomFileName(10) + ".json";
		if (access(filepath_outfile.c_str(), F_OK) != 0)
			break;
	}
	
	std::ofstream file(filepath_outfile.c_str());
	if (file.is_open())
	{
		file << data;
		file.close();
	}
	else
	{
		handler.setStatus(500);
		throw CustomException("Internal Server Error: failed to open file for storing JSON data");
	}
}

std::string	UploadUrlencoded::convertToJSON()
{
	std::string	json;
	int			database_size = database.size();

	json.append("{\r\n");
	for (std::map<std::string, std::string>::iterator it = database.begin(); it != database.end(); it++)
	{
		json.append("\"" + it->first + "\": \"" + it->second + "\"");
		database_size--;
		if (database_size > 0)
			json.append(",\r\n");
	}
	json.append("\r\n}");

	return (json);
}

void	UploadUrlencoded::storeInDatabase()
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

void	UploadUrlencoded::parseBody(char ch)
{
	switch (body_state)
	{
		case key:
			if (ch == CR || ch == LF)
			{
				handler.setStatus(400);
				throw CustomException("Bad request: detected when parsing urlencoded data");
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
				handler.setStatus(400);
				throw CustomException("Bad request: encountered when parsing urlencoded data");
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
			// fall through
		case end_pair:
			storeInDatabase();
	}
}

void	UploadUrlencoded::decode(std::string& sequence)
{
	for (std::string::iterator it = sequence.begin(); it != sequence.end(); it++)
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
				throw CustomException("Bad request: data not properly encoded");
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
				throw CustomException("Bad request: data not properly encoded");
			}
		}
		if (*it == '+')
			*it = ' ';
	}
}


/////////// MAIN METHOD ///////////

void	UploadUrlencoded::uploadData()
{
	if (handler.getChunkDecoder() != NULL)
	{
		input.open(handler.getChunkDecoder()->getUnchunkedDataFile().c_str(), std::ios::ate);
		if (!input.is_open())
		{
			handler.setStatus(500);
			throw CustomException("Internal Server Error: could not open decoded chunked data file");
		}
		std::streamsize file_size = input.tellg();
		input.seekg(0, std::ios::beg);
		char ch;
		for (int i = 0; i < file_size; i++)
		{
			input.read(&ch, 1);
			parseBody(ch);
		}
		storeInDatabase();
		body_read = 1;
		remove(handler.getChunkDecoder()->getUnchunkedDataFile().c_str());
		input.close();
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
	}
	storeInJSON();
}
