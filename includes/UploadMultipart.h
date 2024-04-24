#ifndef UPLOADMULTIPART_H
# define UPLOADMULTIPART_H

# include <string>
# include <map>
# include "CustomException.h"
# include "AUploadModule.h"
# include "RequestHandler.h"

class UploadMultipart: public AUploadModule
{
private:
	// input
	std::ifstream 						input;

	// output
	std::ofstream						outfile;

	// temp vars
	std::string 						temp_value;
	std::string 						temp_key;

	// vars
	std::string							boundary;
	std::map<std::string, std::string>	content_disposition;
	std::string							multipart_content_type;
	int									meta_data_size;
	int									file_data_size;
	int									saved_file_data;
	int									write_size;

	// helper methods
	void	parseBody(char);
	void	identifyBoundary();
	void	checkBoundaryID();
	void	saveContentDispo(char);
	void	checkContentDispoChar(char);
	void	saveContentType(char);
	void	checkContentTypeChar(char);
	void	storeFileData();
	void	storeUnchunkedFileData();
	void	checkCleanTermination(char);
	void	checkFileExistence();
	char	advanceChar();
	void	calcFileSize();

	// states
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

	// constructors
	UploadMultipart();
	UploadMultipart(const UploadMultipart&);
	UploadMultipart& operator=(const UploadMultipart&);

public:
	// constructors and destructors
	explicit UploadMultipart(RequestHandler&);
	~UploadMultipart();

	// main method
	void	uploadData();

};

#endif
