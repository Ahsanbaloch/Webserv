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
	void			unchunkBody();
	void			storeChunkedData();

	int								num_body_reads;
	int								body_parsing_done;
	int								chunk_length;
	int								total_chunk_size;
	bool							trailer_exists; // maybe there is another solution
	std::string						filename;
	std::string						body;
	std::ofstream					temp2;

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
