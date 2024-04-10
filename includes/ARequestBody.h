#ifndef AREQUESTBODY_H
# define AREQUESTBODY_H

#include <string>
#include <map>
#include <fstream>
#include "defines.h"

class RequestHandler;

class ARequestBody
{
protected:
	RequestHandler&				handler;

public:
	ARequestBody(/* args */);
	explicit ARequestBody(RequestHandler&);
	virtual ~ARequestBody();

	virtual void	readBody() = 0;
	void	unchunkBody();
	// void	parsePlainBody();
	// void	checkBoundaryID();
	// void	saveContentDispo();
	// void	saveContentType();
	// void	checkContentType();
	// void	storeContent();
	void	storeChunkedData();
	// void	parseUnchunkedBody();

	// int								bytes_written;
	int								meta_data_size;
	int								file_data_size;
	int								saved_file_data;
	int								num_body_reads;
	int								write_size;
	int								body_parsing_done;
	int								chunk_length;
	bool							trailer_exists; // maybe there is another solution
	std::string						filename;
	std::string						body;
	std::ofstream					temp;
	std::ofstream					temp2;

	std::map<std::string, std::string>	content_disposition;
	std::string						multipart_content_type;

	enum {
		body_start = 0,
		chunk_size,
		chunk_size_cr,
		chunk_extension,
		chunk_data,
		chunk_data_cr,
		chunk_trailer,
		chunk_trailer_cr,
		body_end_cr,
		body_end
	} te_state;

};


#endif
