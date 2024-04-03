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
	void	parseChunkedBody();
	void	parsePlainBody();

	int								body_parsing_done;
	int								chunk_length;
	bool							trailer_exists; // maybe there is another solution
	std::string						body;


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
