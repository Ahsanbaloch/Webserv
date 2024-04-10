#ifndef REQUESTBODY_H
# define REQUESTBODY_H

#include <string>
#include <map>
#include <fstream>
#include "defines.h"

class RequestHandler;

class RequestBody
{
private:
	RequestHandler&				handler;

public:
	RequestBody(/* args */);
	explicit RequestBody(RequestHandler&);
	~RequestBody();

	void	readBody();
	void	unchunkBody();
	void	parsePlainBody();
	void	checkBoundaryID();
	void	saveContentDispo();
	void	saveContentType();
	void	checkContentType();
	void	storeContent();
	void	storeChunkedData();
	void	parseUnchunkedBody();

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

	std::string 					boundary;
	std::map<std::string, std::string>	content_disposition;
	std::string						multipart_content_type;

	enum {
		unknown = 0,
		multipart_form,
		text_plain,
		urlencoded
	} content_type;

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

	enum {
		mp_start = 0,
		mp_boundary_id,
		mp_content_dispo,
		mp_content_type,
		mp_empty_line,
		mp_content,
		mp_boundary_end
	} mp_state;

	enum {
		begin = 0,
		type,
		var_key,
		var_value
	} content_dispo_state;

	enum {
		begin2 = 0,
		type_name
	} content_type_state;

};


#endif
