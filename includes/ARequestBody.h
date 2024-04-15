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
	RequestHandler&	handler;

	// vars
	int				chunk_length;
	int				total_chunk_size;
	std::string		filename;
	std::ofstream	temp_chunked;

	// flags
	bool			body_read;
	bool			body_parsing_done;
	bool			trailer_exists;

	// states
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

	// constructors
	ARequestBody();
	ARequestBody(const ARequestBody&);
	ARequestBody& operator=(const ARequestBody&);

public:
	// constructors & destructors
	explicit ARequestBody(RequestHandler&);
	virtual ~ARequestBody();

	// getters
	bool			getBodyProcessed() const;

	// methods
	virtual void	readBody() = 0;
	void			unchunkBody();
	void			storeChunkedData();

};

#endif
