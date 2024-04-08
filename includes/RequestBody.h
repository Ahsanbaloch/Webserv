#ifndef REQUESTBODY_H
# define REQUESTBODY_H

#include <string>
#include <fstream>

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
	void	parseChunkedBody();
	void	parsePlainBody();
	void	checkBoundaryID();
	void	saveContentDispo();
	void	saveContentType();
	void	checkContentType();

	int								body_parsing_done;
	int								chunk_length;
	bool							trailer_exists; // maybe there is another solution
	std::string						body;
	std::ofstream					temp;

	std::string 					boundary;

	enum {
		unknown = 0,
		multipart_form
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

};


#endif
