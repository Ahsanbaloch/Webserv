
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

std::string GETResponse::createHTMLPage(const std::string& directory_name, const std::vector<std::string>& items)
{
	std::string html_template = readHTMLTemplateFile("www/dir_list_template.html");
	std::ostringstream directory_items;
	for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
	{
	 std::string link = "/" + *it;
    directory_items << "<li class=\"list-group-item\"><a href=\"" << directory_name << link << "\">" << *it << "</a> "
        << "<button class=\"btn btn-danger float-right\" onclick=\"this.firstChild.classList.remove('d-none'); sendDeleteRequest('" << directory_name << link << "');\">"
        << "<span class=\"spinner-border spinner-border-sm d-none\" role=\"status\"></span> Delete</button></li>";
	}

	size_t pos = html_template.find("{directory_name}");
	if (pos != std::string::npos)
		html_template.replace(pos, 16, directory_name);
	pos = html_template.find("{directory_items}");
	if (pos != std::string::npos)
		html_template.replace(pos, 17, directory_items.str());

	return (html_template);
}

std::string GETResponse::getBodyFromDir()
{
	std::string body;
	std::vector<std::string> items;
	DIR *directory;
	struct dirent *entry;

	std::string directory_name = handler.getLocationConfig().path;
	directory = opendir((handler.getLocationConfig().root + directory_name).c_str());
	if (directory != NULL)
	{
		entry = readdir(directory);
		while (entry)
		{
			if (strcmp(entry->d_name, ".gitkeep") != 0)
				items.push_back(entry->d_name);
			entry = readdir(directory);
		}
		closedir(directory);
	}
	else
	{
		handler.setStatus(404);
		throw CustomException("Not found");
	}
	body = createHTMLPage(directory_name, items);
	return (body);
}

std::string GETResponse::createBody()
{
	std::string body;

	if (auto_index)
		body = getBodyFromDir();
	else
	{
		openBodyFile(full_file_path);
		body = createBodyChunk();
		chunked_body = 1;
	}
	return (body);
}

std::string	GETResponse::createHeaderFields()
{
	std::string	header;
	std::string mime_type = identifyMIME();

	header.append("Content-Type: " + mime_type + "\r\n");
	if (auto_index)
	{
		header.append("Content-Length: ");
		header.append(toString(body.size()) + "\r\n");
	}
	else
		header.append("Content-Length: " + toString(body_size) + "\r\n");
	header.append("\r\n");
	return (header);
}

void	GETResponse::determineOutput()
{
	if (handler.getHeaderInfo().getFileExtension().empty())
	{
		if (handler.getLocationConfig().autoIndex)
			auto_index = 1;
		else
		{
			handler.setStatus(404);
			throw CustomException("Not Found");
		}
	}
	else
	{
		file_type = handler.getHeaderInfo().getFileExtension();
		full_file_path = handler.getLocationConfig().root + handler.getHeaderInfo().getPath();
	}
}

std::string	GETResponse::identifyMIME()
{	
	if (auto_index)
		return ("text/html");
	else if (file_type == ".html")
		return ("text/html");
	else if (file_type == ".jpeg" || file_type == ".jpg")
		return ("image/jpeg");
	else if (file_type == ".ico")
		return ("image/x-icon");
	else if (file_type == ".png")
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
	else if (file_type == ".tif")
		return (" image/tiff");
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
		body = "";
	else
		body = createBody();
	
	header_fields = createHeaderFields();
}
