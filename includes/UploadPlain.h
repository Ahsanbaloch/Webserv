#ifndef UPLOADPLAIN_H
# define UPLOADPLAIN_H

# include "AUploadModule.h"
# include "RequestHandler.h"
# include "defines.h"
# include "utils.tpp"

class UploadPlain: public AUploadModule
{
private:
	// input
	std::ifstream	input;

	// output
	std::ofstream	outfile;

	// vars
	int				body_bytes_consumed;

	// helper methods
	void			checkFilepath();
	void			openFiles();

	// constructors
	UploadPlain();
	UploadPlain(const UploadPlain&);
	UploadPlain& operator=(const UploadPlain&);

public:
	// constructors and desctructors
	explicit UploadPlain(RequestHandler&);
	~UploadPlain();

	// main method
	void			uploadData();
};


#endif
