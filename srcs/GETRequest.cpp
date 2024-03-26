
#include "GETRequest.h"

GETRequest::GETRequest(RequestHandler&)
{
}

GETRequest::GETRequest(/* args */)
{
}

GETRequest::~GETRequest()
{
}

std::string	GETRequest::createStatusLine(RequestHandler& handler)
{
	std::string status_line;
	std::ostringstream status_conversion;

	status_line.append("HTTP/1.1 "); // alternative handler.head.version
	status_conversion << handler.status;
	status_line.append(status_conversion.str());
	status_line.append(" \r\n");  //A server MUST send the space that separates the status-code from the reason-phrase even when the reason-phrase is absent (i.e., the status-line would end with the space)
	return (status_line);
}


std::string	GETRequest::getBodyFromFile(RequestHandler& handler)
{
	std::string body;

	std::ifstream file(handler.file_path); // Open the file
	if (!file.is_open()) 
	{
		handler.status = 404;
		throw CustomException("Not found");
	}
	std::stringstream buffer;
	buffer << file.rdbuf();

	body = buffer.str();

	file.close(); 
	return (body);
}

std::string GETRequest::getBodyFromDir(RequestHandler& handler) // probably create some html for it
{
	std::string body;
	DIR *directory;
	struct dirent *entry;

	directory = opendir((handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].root + handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].path).c_str());
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
		handler.status = 404;
		throw CustomException("Not found");
	}
	return (body);
}


std::string GETRequest::createBody(RequestHandler& handler)
{
	std::string body;

	if (handler.status >= 400) // check if error was identified (or is this handled somewhere else?)
		; // From configData get specific info about which page should be displayed
		// look up file and read content into response body
	if (handler.autoindex == 1)
		body = getBodyFromDir(handler);
	else
		body = getBodyFromFile(handler);
	return (body);
}

std::string	GETRequest::createHeaderFields(RequestHandler& handler, std::string body)
{
	std::string	header;

	if (handler.url_relocation)
		header.append("Location: " + handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].redirect + "\r\n");
	else
	{
		std::string mime_type = identifyMIME(handler); // should not be called if we have a url redirection
		if (mime_type.empty()) // only check when body should be sent?
		{
			handler.status = 415;
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
			header.append("\r\n");
		}
	}
	return (header);
}

void	GETRequest::checkRedirects(RequestHandler& handler)
{
	// if the request is not for a file (otherwise the location has already been found)
	if (!checkFileType(handler))
	{
		// check if file constructed from root, location path and index exists
		if (checkFileExistence(handler) == 0)
		{
			// if file does exist, search again for correct location
			// findLocationBlock(handler); //should the root be taken into account when rechecking the location Block?
			handler.findLocationBlock();
			handler.file_path = handler.header.redirected_path;
			handler.file_type = handler.file_path.substr(handler.file_path.find('.') + 1); // create a function for that in case it is not a file type
		}
		else
		{
			if (handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].autoIndex)
				handler.autoindex = 1;
			else
			{
				handler.status = 404;
				throw CustomException("Not Found");
			}
		}
	}
	else
	{
		// file_type is already set by checkFileType
		handler.file_path = handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].root + "/" + handler.header.getPath();
	}
	std::cout << "location selected: " << handler.selected_location << std::endl;
}

Response	*GETRequest::createResponse(RequestHandler& handler)
{
	Response *response = new Response; // needs to be delete somewhere

	// check for direct redirects and internal redirects
	if (!handler.url_relocation)
		checkRedirects(handler);

	// check allowed methods for the selected location
	// if (!handler.getServerConfig()[handler.selected_server].locations[handler.selected_location].getAllowed)
		// throw exception

	response->status_line = createStatusLine(handler);
	if ((handler.status >= 100 && handler.status <= 103) || handler.status == 204 || handler.status == 304 || handler.status == 307)
		response->body = ""; // or just initialize it like that // here no body should be created
	else
		response->body = createBody(handler);
	
	response->header_fields = createHeaderFields(handler, response->body);

	// The presence of a message body in a response depends on both the request method to which it is responding and the response status code. 
	// e.g. POST 200 is different from GET 200

	// A server MUST NOT send a Transfer-Encoding header field in any response with a status code of 1xx (Informational) or 204 (No Content)
	// any response with a 1xx (Informational), 204 (No Content), or 304 (Not Modified) status code is always terminated by the first empty line after the header fields --> no body
	return (response);
}


std::string	GETRequest::identifyMIME(RequestHandler& handler)
{
	// also check against accept header? --> return 406 if the requirement cannot be satisfied
	// how to best identifyMIME?
	if (handler.autoindex) // probably also rather going to be html
		return ("text/plain");
	else if (handler.file_type == "html")
		return ("text/html");
	else if (handler.file_type == "png" || handler.file_type == "ico")
		return ("image/png");
	else
		return (""); // what should be the default return?
}
