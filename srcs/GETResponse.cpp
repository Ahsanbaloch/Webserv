
#include "GETResponse.h"

/////////// CONSTRUCTORS & DESTRUCTORS ///////////

GETResponse::GETResponse()
	: AResponse(), auto_index(0)
{
}

GETResponse::GETResponse(RequestHandler& src)
	: AResponse(src), auto_index(0)
{
}

GETResponse::~GETResponse()
{
}

GETResponse::GETResponse(const GETResponse& src)
	: AResponse(src), auto_index(src.auto_index)
{
}

GETResponse& GETResponse::operator=(const GETResponse& src)
{
	if (this != &src)
	{
		AResponse::operator=(src);
		auto_index = src.auto_index;
	}
	return (*this);
}


/////////// HELPER METHODS ///////////

std::string	GETResponse::getBodyFromFile()
{
	std::string body;

	std::ifstream file(full_file_path); // Open the file
	if (!file.is_open()) 
	{
		handler.setStatus(404);
		throw CustomException("Not found");
	}
	std::stringstream buffer;
	buffer << file.rdbuf();

	body = buffer.str();

	file.close(); 
	return (body);
}

std::string GETResponse::getBodyFromDir() // probably create some html for it
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
		body = getBodyFromFile();
	return (body);
}

std::string	GETResponse::createHeaderFields(std::string body) // probably don't need parameter anymore
{
	std::string	header;

	if (!handler.getLocationConfig().redirect.empty())
		header.append("Location: " + handler.getLocationConfig().redirect + "\r\n");
	else
	{
		std::string mime_type = identifyMIME(); // should not be called if we have a url redirection
		if (mime_type.empty()) // only check when body should be sent?
		{
			handler.setStatus(415);
			throw CustomException("Unsupported Media Type");
		}
		else
		{
			std::ostringstream length_conversion;
			length_conversion << body.size();
			header.append("Content-Type: " + mime_type + "\r\n");
			header.append("Content-Length: "); // alternatively TE: chunked?
			header.append(length_conversion.str() + "\r\n");
			// header.append("Location: url"); // redirect client to a different url or new path 
			// what other headers to include?
			// send Repsonses in Chunks?
			// header.append("Transfer-Encoding: chunked");
			// header.append("Cache-Control: no-cache");
			// header.append("Set-Cookie: preference=darkmode; Domain=example.com");
			// header.append("Server: nginx/1.21.0");
			// header.append("Expires: Sat, 08 May 2023 12:00:00 GMT"); // If a client requests the same resource before the expiration date has passed, the server can return a cached copy of the resource.
			// header.append("Last-Modified: Tue, 04 May 2023 16:00:00 GMT"); // This header specifies the date and time when the content being sent in the response was last modified. This can be used by clients to determine if the resource has changed since it was last requested.
			// header.append("ETag: "abc123""); //This header provides a unique identifier for the content being sent in the response. This can be used by clients to determine if the resource has changed since it was last requested, without having to download the entire resource again.
			// header.append("Keep-Alive: timeout=5, max=100"); // used to enable persistent connections between the client and the server, allowing multiple requests and responses to be sent over a single TCP connection
			// Access-Control-Allow-Origin; X-Frame-Options; X-XSS-Protection; Referrer-Policy; X-Forwarded-For; X-Powered-By; 
			// header.append("\r\n");
		}
	}
	header.append("\r\n");
	return (header);
}

void	GETResponse::checkRedirectedLocationBlock()
{
	if (!handler.getLocationConfig().GET)
	{
		handler.setStatus(405);
		throw CustomException("Method Not Allowed");
	}
	if (!handler.getLocationConfig().redirect.empty())
	{
		std::string redirect_url = REDIRECTResponse::removeSchemeFromURL(handler.getLocationConfig().redirect);
		if (redirect_url == "localhost:" + toString(handler.getServerConfig()[0].port) + org_path)
		{
			handler.setStatus(508);
			throw CustomException("Loop Detected");
		}
		handler.setStatus(307);
	}
		
}

void	GETResponse::checkInternalRedirects()
{
	// if the request is not for a file (otherwise the location has already been found)
	if (handler.getHeaderInfo().getFileExtension().empty())
	{
		full_file_path = buildPathFromLocationIndex();
		// check if file constructed from root, location path and index exists
		if (access(full_file_path.c_str(), F_OK) == 0)
		{
			internal_redirect = 1;
			org_path = handler.getLocationConfig().path;
			handler.findLocationBlock(); // changing the selected location here might create a wild pointer issue
			checkRedirectedLocationBlock();
			full_file_path = handler.getLocationConfig().root + full_file_path.substr(handler.getLocationConfig().root.length());
			std::cout << "full file path: " << full_file_path << std::endl;
			file_type = full_file_path.substr(full_file_path.find_last_of('.') + 1); // create a function for that in case it is not a file type
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
	std::cout << "location selected: " << handler.getSelectedLocation() << std::endl;
}

std::string	GETResponse::identifyMIME()
{
	// also check against accept header? --> return 406 if the requirement cannot be satisfied
	// how to best identifyMIME?
	if (auto_index) // probably also rather going to be html
		return ("text/plain");
	else if (file_type == "html")
		return ("text/html");
	else if (file_type == "jpeg")
		return ("image/jpeg");
	else if (file_type == "png" || file_type == "ico")
		return ("image/png");
	else
		return (""); // what should be the default return?
}

/////////// MAIN METHODS ///////////

void	GETResponse::createResponse()
{
	// check for direct redirects and internal redirects
	if (!handler.getLocationConfig().redirect.empty())
		handler.setStatus(307);
	else
		checkInternalRedirects();

	// check allowed methods for the selected location
	// if (!handler.getLocationConfig().GET)
	// {
	// 	handler.setStatus(405);
	// 	throw CustomException("Method Not Allowed");
	// }

	status_line = createStatusLine();
	if ((handler.getStatus() >= 100 && handler.getStatus() <= 103) || handler.getStatus() == 204 || handler.getStatus() == 304 || handler.getStatus() == 307)
		body = ""; // or just initialize it like that // here no body should be created
	else
		body = createBody();
	
	header_fields = createHeaderFields(body);

	// The presence of a message body in a response depends on both the request method to which it is responding and the response status code. 
	// e.g. POST 200 is different from GET 200

	// A server MUST NOT send a Transfer-Encoding header field in any response with a status code of 1xx (Informational) or 204 (No Content)
	// any response with a 1xx (Informational), 204 (No Content), or 304 (Not Modified) status code is always terminated by the first empty line after the header fields --> no body
	// return (response);
}
