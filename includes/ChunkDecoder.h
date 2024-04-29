#ifndef CHUNKDECODER_H
# define CHUNKDECODER_H

# include <string>
# include <fstream>

class RequestHandler;

class ChunkDecoder
{
private:
	RequestHandler&	handler;

	// vars
	int				chunk_length;
	int				total_chunk_size;
	std::ofstream	temp_unchunked;
	std::string		temp_filename_unchunked;
	
	// flags
	bool			trailer_exists;
	bool			body_unchunked;

	// helper methods
	void			storeChunkedData();
	void			checkMaxBodySize();

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
	ChunkDecoder();
	ChunkDecoder(const ChunkDecoder&);
	ChunkDecoder& operator=(const ChunkDecoder&);

public:
	// constructors & destructors
	explicit ChunkDecoder(RequestHandler&);
	~ChunkDecoder();

	// getters
	std::string			getUnchunkedDataFile() const;
	int					getTotalChunkSize() const;
	bool				getDecodingStatus() const;

	// main method
	void				unchunkBody();

};

#endif
