#ifndef REQUESTBODY_H
# define REQUESTBODY_H

#include <string>

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
	void	processChunkedBody();

	int								body_parsing_done;
	int								chunk_length;
	std::string						body;


	enum {
		body_start = 0,
		chunk_size,
		chunk_size_cr,
		chunk_extension,
		chunk_data,
		chunk_data_cr,
		chunk_trailer,
		body_end
	} te_state;

};


#endif
