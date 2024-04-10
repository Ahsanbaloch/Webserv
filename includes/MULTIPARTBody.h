#ifndef MULTIPARTBODY_H
# define MULTIPARTBODY_H

#include <string>
#include "ARequestBody.h"
#include "RequestHandler.h"

class MULTIPARTBody: public ARequestBody
{
private:
	std::string							boundary;
	int									meta_data_size;
	int									file_data_size;
	int									saved_file_data;
	int									write_size;
	std::ofstream						temp;
	std::map<std::string, std::string>	content_disposition;
	std::string							multipart_content_type;

public:
	explicit MULTIPARTBody(RequestHandler&);
	~MULTIPARTBody();

	void	readBody();
	void	identifyBoundary();
	void	parseBody();
	void	storeFileData();
	void	saveContentDispo();
	void	saveContentType();
	void	checkBoundaryID();

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

};


#endif
