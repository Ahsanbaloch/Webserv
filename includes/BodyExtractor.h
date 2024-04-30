#ifndef BODYEXTRACTOR_H
# define BODYEXTRACTOR_H

# include <fstream>
# include "defines.h"
# include "CustomException.h"
# include "utils.h"
# include "utils.tpp"

class RequestHandler;

class BodyExtractor
{
private:
	RequestHandler&	handler;

	std::string		temp_body_filepath;
	std::ofstream	outfile;
	int				body_bytes_consumed;
	bool			extraction_status;

	// constructors
	BodyExtractor();
	BodyExtractor(const BodyExtractor&);
	BodyExtractor& operator=(const BodyExtractor&);

public:
	explicit BodyExtractor(RequestHandler&);
	~BodyExtractor();

	// getters
	std::string		getTempBodyFilepath() const;
	bool			getExtractionStatus() const;

	// methods
	void			extractBody();
};



#endif
