#ifndef AUPLOADMODULE_H
# define AUPLOADMODULE_H

#include <string>
#include <map>
#include <fstream>
#include "defines.h"

class RequestHandler;

class AUploadModule
{
protected:
	RequestHandler&	handler;

	// vars
	// int				chunk_length;
	// int				total_chunk_size;
	std::string			filename; // move to children? (if not needed in all sub-classes)
	// std::ofstream	temp_chunked;

	// flags
	bool			body_read;
	bool			body_parsing_done; // only needed in Multipart class
	// bool			trailer_exists;

	// // states
	// enum {
	// 	body_start = 0,
	// 	chunk_size,
	// 	chunk_size_cr,
	// 	chunk_extension,
	// 	chunk_data,
	// 	chunk_data_cr,
	// 	chunk_trailer,
	// 	chunk_trailer_cr,
	// 	body_end_cr,
	// 	body_end
	// } te_state;

	// constructors
	AUploadModule();
	AUploadModule(const AUploadModule&);
	AUploadModule& operator=(const AUploadModule&);

public:
	// constructors & destructors
	explicit AUploadModule(RequestHandler&);
	virtual ~AUploadModule();

	// getters
	bool			getBodyProcessed() const;

	// methods
	virtual void	readBody() = 0;
	// void			unchunkBody();
	// void			storeChunkedData();

};

#endif
