#include "AResponse.h"
#include "RequestHandler.h"

///////// CONSTRUCTORS & DESTRUCTORS ///////////

AResponse::AResponse()
	: handler(*new RequestHandler()), body_size(0), file_position(0), file_pos_offset(0), chunked_body(0)
{
}

AResponse::AResponse(RequestHandler& request_handler) 
	: handler(request_handler), body_size(0), file_position(0), file_pos_offset(0), chunked_body(0)
{
}

AResponse::~AResponse()
{
}

AResponse::AResponse(const AResponse& src)
	: handler(src.handler)
{
	file_type = src.file_type;
	full_file_path = src.full_file_path;
	body = src.body;
	status_line = src.status_line;
	header_fields = src.header_fields;
	body_size = src.body_size;
	file_position = src.file_position;
	file_pos_offset = src.file_pos_offset;
	chunked_body = src.chunked_body;
}

AResponse& AResponse::operator=(const AResponse& src)
{
	if (this != &src)
	{
		handler = src.handler;
		file_type = src.file_type;
		full_file_path = src.full_file_path;
		body = src.body;
		status_line = src.status_line;
		header_fields = src.header_fields;
		body_size = src.body_size;
		file_position = src.file_position;
		file_pos_offset = src.file_pos_offset;
		chunked_body = src.chunked_body;
	}
	return (*this);
}


////////// GETTERS ///////////

std::string AResponse::getRespondsHeaderFields() const
{
	return (header_fields);
}

std::string AResponse::getResponseBody() const
{
	return (body);
}

std::string AResponse::getResponseStatusLine() const
{
	return (status_line);
}

std::string AResponse::getFullFilePath() const
{
	return (full_file_path);
}

bool	AResponse::getChunkedBodyStatus() const
{
	return (chunked_body);
}

std::ifstream&	AResponse::getBodyFile()
{
	return (body_file);
}

std::streampos	AResponse::getBodySize() const
{
	return (body_size);
}

std::streampos	AResponse::getFilePosition() const
{
	return (file_position);
}


std::streampos	AResponse::getFilePosOffset() const
{
	return (file_pos_offset);
}


///////// CLASS METHODS ///////////

void	AResponse::openBodyFile(std::string filepath)
{
	full_file_path = filepath;
	body_file.open(full_file_path.c_str(), std::ios::binary);
	if (!body_file.is_open()) 
	{
		handler.setStatus(404);
		throw CustomException("Not found");
	}
	body_file.seekg(0, std::ios::end);
	body_size = body_file.tellg();
	body_file.seekg(std::ios::beg);
}

std::string	AResponse::createBodyChunk()
{
	char	buffer[BUFFER_SIZE];

	body_file.seekg(file_position);
	body_file.read(buffer, BUFFER_SIZE);
	if (body_file.fail())
	{
		if (!body_file.eof())
		{
			handler.setStatus(500);
			body_file.close();
			throw CustomException("Internal Server Error: failed to create response chunk");
		}
	}
	std::streampos bytes_read = body_file.gcount();
	std::string chunk_content(buffer, bytes_read);
	return (chunk_content);
}

std::string	AResponse::createStatusLine()
{
	std::string status_line;

	status_line.append("HTTP/1.1 ");
	status_line.append(toString(handler.getStatus()));
	status_line.append(" \r\n"); //A server MUST send the space that separates the status-code from the reason-phrase even when the reason-phrase is absent (i.e., the status-line would end with the space)
	return (status_line);
}

std::string AResponse::readHTMLTemplateFile(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	if (!file)
	{
		handler.setStatus(500);
		throw CustomException("Internal Server Error: failed to open file");
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return (buffer.str());
}


///////// MAIN METHODS ///////////

void	AResponse::incrementFilePosition(std::streampos position_increment)
{
	file_position += position_increment;
}
