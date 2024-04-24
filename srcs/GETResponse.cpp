
#include "GETResponse.h"

/////////// CONSTRUCTORS & DESTRUCTORS ///////////

GETResponse::GETResponse()
	: AResponse(), file_position(0), file_size(0), auto_index(0)
{
}

GETResponse::GETResponse(RequestHandler& src)
	: AResponse(src), file_position(0), file_size(0), auto_index(0)
{
}

GETResponse::~GETResponse()
{
}

GETResponse::GETResponse(const GETResponse& src)
	: AResponse(src), file_position(src.file_position),  file_size(src.file_size), auto_index(src.auto_index)
{
}

GETResponse& GETResponse::operator=(const GETResponse& src)
{
	if (this != &src)
	{
		AResponse::operator=(src);
		file_position = src.file_position;
		file_size = src.file_size;
		auto_index = src.auto_index;
	}
	return (*this);
}


// void	GETResponse::decrementFilePosition(std::streampos bytes_not_read)
// {
// 	file_position -= bytes_not_read;
// }

void	GETResponse::incrementFilePosition(std::streampos position_increment)
{
	file_position += position_increment;
}

//////////// GETTTER ///////////

std::streampos	GETResponse::getFilePosition() const
{
	return (file_position);
}

std::ifstream&	GETResponse::getInputFile()
{
	return (input_file);
}

std::streampos	GETResponse::getFileSize() const
{
	return (file_size);
}


/////////// HELPER METHODS ///////////

std::string	GETResponse::getBodyFromFile()
{
	// std::string			chunk_termination;
	char 				buffer[BUFFER_SIZE];
	
	input_file.seekg(file_position);
	input_file.read(buffer, BUFFER_SIZE);
	if (input_file.fail())
	{
		if (!input_file.eof())
		{
			handler.setStatus(500);
			input_file.close();
			throw CustomException("Internal Server Error");
		}
	}
	std::streampos bytes_read = input_file.gcount();
	std::string chunk_content(buffer, bytes_read);

	// file_position += bytes_read;
	// if (file_position == file_size)
	// {
	// 	response_complete = 1;
	// 	// chunk_termination = "\r\n0\r\n\r\n";
	// 	input_file.close();
	// }
	// else
	// 	chunk_termination = "\r\n";
	
	// std::string chunk_length = toHex(bytes_read) + "\r\n";
	// return (chunk_length + chunk_content + chunk_termination);
	handler.bytes_to_send = bytes_read;
	return (chunk_content);
}

std::string GETResponse::getBodyFromDir()
{
	std::string body;
	DIR *directory;
	struct dirent *entry;

	directory = opendir((handler.getLocationConfig().root + handler.getLocationConfig().path).c_str());
	if (directory != NULL)
	{
		entry = readdir(directory);
		while (entry)
		{
			body.append(entry->d_name);
			body.append("\n");
			entry = readdir(directory);
		}
		closedir(directory);
	}
	else
	{
		handler.setStatus(404);
		throw CustomException("Not found");
	}
	return (body);
}

std::string GETResponse::createBody()
{
	std::string body;

	if (auto_index)
		body = getBodyFromDir();
	else
	{
		input_file.open(full_file_path, std::ios::binary);
		if (!input_file.is_open()) 
		{
			handler.setStatus(404);
			throw CustomException("Not found");
		}
		input_file.seekg(0, std::ios::end);
		file_size = input_file.tellg();
		input_file.seekg(std::ios::beg);
		std::cout << "file size: " << file_size << std::endl;
		
		body = getBodyFromFile();
		chunked_body = 1;
	}
	return (body);
}

std::string	GETResponse::createHeaderFields(std::string body) // probably don't need parameter anymore
{
	std::string	header;
	std::string mime_type = identifyMIME();


	header.append("Content-Type: " + mime_type + "\r\n");
	// header.append("Content-Length: "); // alternatively TE: chunked?
	// header.append(toString(body.size()) + "\r\n");
	// what other headers to include?
	// send Repsonses in Chunks
	if (auto_index)
	{
		header.append("Content-Length: "); // alternatively TE: chunked?
		header.append(toString(body.size()) + "\r\n");
	}
	else
	{
		// header.append("Transfer-Encoding: chunked\r\n");
		header.append("Content-Length: " + std::to_string(file_size) + "\r\n");
	}
	// header.append("Cache-Control: no-cache");
	// header.append("Set-Cookie: preference=darkmode; Domain=example.com");
	// header.append("Server: nginx/1.21.0");
	// header.append("Expires: Sat, 08 May 2023 12:00:00 GMT"); // If a client requests the same resource before the expiration date has passed, the server can return a cached copy of the resource.
	// header.append("Last-Modified: Tue, 04 May 2023 16:00:00 GMT"); // This header specifies the date and time when the content being sent in the response was last modified. This can be used by clients to determine if the resource has changed since it was last requested.
	// header.append("ETag: "abc123""); //This header provides a unique identifier for the content being sent in the response. This can be used by clients to determine if the resource has changed since it was last requested, without having to download the entire resource again.
	// header.append("Keep-Alive: timeout=5, max=100"); // used to enable persistent connections between the client and the server, allowing multiple requests and responses to be sent over a single TCP connection
	// Access-Control-Allow-Origin; X-Frame-Options; X-XSS-Protection; Referrer-Policy; X-Forwarded-For; X-Powered-By; 

	header.append("\r\n");
	return (header);
}

void	GETResponse::determineOutput()
{
	// if the request is not for a file (otherwise the location has already been found)
	if (handler.getHeaderInfo().getFileExtension().empty())
	{
		if (handler.getIntRedirStatus())
		{
			full_file_path = handler.getNewFilePath(); // may adjust after config integration
			file_type = full_file_path.substr(full_file_path.find_last_of('.')); 
		}
		else
		{
			if (handler.getLocationConfig().autoIndex)
			{
				printf("auto index found\n");
				auto_index = 1;
			}
			else
			{
				handler.setStatus(404);
				throw CustomException("Not Found");
			}
		}
	}
	else
	{
		file_type = handler.getHeaderInfo().getFileExtension();
		full_file_path = handler.getLocationConfig().root + "/" + handler.getHeaderInfo().getPath();
	}
}

std::string	GETResponse::identifyMIME()
{	
	if (auto_index) // probably also rather going to be html
		return ("text/plain");
	else if (file_type == ".html")
		return ("text/html");
	else if (file_type == ".jpeg" || file_type == ".jpg")
		return ("image/jpeg");
	else if (file_type == ".png" || file_type == ".ico")
		return ("image/png");
	else if (file_type == ".bin")
		return ("application/octet-stream");
	else if (file_type == ".bmp")
		return ("image/bmp");
	else if (file_type == ".css")
		return ("text/css");
	else if (file_type == ".csv")
		return ("text/csv");
	else if (file_type == ".gif")
		return ("image/gif");
	else if (file_type == ".js")
		return ("text/javascript");
	else if (file_type == ".json")
		return ("application/json");
	else if (file_type == ".mp3")
		return ("audio/mpeg");
	else if (file_type == ".mp4")
		return ("video/mp4");
	else if (file_type == ".mpeg")
		return ("video/mpeg");
	else if (file_type == ".pdf")
		return ("application/pdf");
	else if (file_type == ".svg")
		return ("image/svg+xml");
	else if (file_type == ".txt")
		return ("text/plain");
	else if (file_type == ".wav")
		return ("audio/wav");
	else if (file_type == ".xhtml")
		return ("application/xhtml+xml");
	else
	{
		handler.setStatus(415);
		throw CustomException("Unsupported Media Type");
	}
}


/////////// MAIN METHODS ///////////

void	GETResponse::createResponse()
{
	determineOutput();

	status_line = createStatusLine();
	if ((handler.getStatus() >= 100 && handler.getStatus() <= 103) || handler.getStatus() == 204 || handler.getStatus() == 304)
		body = ""; // or just initialize it like that // here no body should be created
	else
		body = createBody();
	
	header_fields = createHeaderFields(body);
	handler.bytes_to_send = status_line.size() + body.size() + header_fields.size();
}
