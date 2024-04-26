
#include "CGIResponse.h"

CGIResponse::CGIResponse(RequestHandler& src)
	: AResponse(src)
{

}

CGIResponse::CGIResponse(/* args */)
	: AResponse()
{
}

CGIResponse::~CGIResponse()
{
}

void	CGIResponse::readCGIHeader()
{
	std::string	line;

	while(std::getline(body_file, line))
	{
		std::cout << "line: " << line << std::endl;
		if (line.find("Status") != std::string::npos)
		{
			std::string status;
			for (std::string::iterator it = line.begin(); it != line.end(); it++)
			{
				if (isdigit(*it))
					status += *it;
			}
			handler.setStatus(toInt(status));
			if (toInt(status) >= 400)
				throw CustomException("CGI Error");
		}
		else if (!line.empty())
			header_fields.append(line + "\r\n");
		else
			break;
		
	}
	file_position = body_file.tellg();
	file_pos_offset = file_position;
	body_size = body_size - file_position;
}


void	CGIResponse::createResponse()
{
	openBodyFile(handler.getCGI()->getCGIOutput());
	readCGIHeader();
	if (header_fields.find("Location") != std::string::npos)
		body = "";
	else if (header_fields.find("Content-Type") != std::string::npos)
	{
		if (header_fields.find("Content-Length") == std::string::npos)
			header_fields.append("Content-Length: " + toString(body_size) + "\r\n");
		body = createBodyChunk();
		chunked_body = 1;
	}
	else
	{
		handler.setStatus(500);
		throw CustomException("CGI Error"); // should this be done?
	}
	header_fields.append("\r\n");
	status_line = createStatusLine();

	// need to read the header line of the CGI response
	// needs to be at least one line seperated by newline
	// may include status code
		
		// Content-Type --> if html has been created (e.g. form); status: 200
		// Location --> local resource --> indicates to server to reprocess request with the new path
			// if internal redircect --> reset path, filename and file extension and reseach for location
			// find new Location
			// make getResponse
			// status code 303 (See other); needs to be sent in this case
			// what if file does not exist?

	// if (handler.getHeaderInfo().getMethod() == "GET")
	// {
		//	Content-Type --> if html has been created (e.g. form); status: 200
	// }
	// else
	// {
	// 	handler.setStatus(200);

	// }
}
