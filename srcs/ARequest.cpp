#include "ARequest.h"
#include "GETRequest.h"
#include "RequestHandler.h"

ARequest::ARequest(/* args */)
{
}

ARequest::~ARequest()
{
}

// instead of erasing all server blocks, could also store the index (similar to Location Blocks)
void	ARequest::findServerBlock(RequestHandler& handler)
{
	std::vector<t_server_config>::iterator it = handler.server_config.begin();
	for (std::vector<t_server_config>::iterator it2 = handler.server_config.begin(); it2 != handler.server_config.end(); it2++)
	{
		if (it2 == it || it2->serverName == handler.header.header_fields["host"])
			it = it2;
		else
		{
			handler.server_config.erase(it2);
			it2--;
		}
	}
	if (it != handler.server_config.begin())
		handler.server_config.erase(handler.server_config.begin());
}

std::vector<std::string>	ARequest::splitPath(std::string input, char delim)
{
	std::istringstream			iss(input);
	std::string					item;
	std::vector<std::string>	result;
	
	while (std::getline(iss, item, delim))
		result.push_back(item);
	return (result);
}

int	ARequest::calcMatches(std::vector<std::string>& uri_path_items, std::vector<std::string>& location_path_items)
{
	// printf("splitted string\n");
	// for (std::vector<std::string>::iterator it = uri_path_items.begin(); it != uri_path_items.end(); it++)
	// {
	// 	std::cout << "string uri: " << *it << std::endl;
	// }
	// for (std::vector<std::string>::iterator it = location_path_items.begin(); it != location_path_items.end(); it++)
	// {
	// 	std::cout << "string location: " << *it << std::endl;
	// }
	int	matches = 0;

	for (std::vector<std::string>::iterator it = location_path_items.begin(); it != location_path_items.end(); it++)
	{
		if (*it != uri_path_items[matches])
			break;
		matches++;
	}
	std::cout << "Num matches: " << matches << std::endl;
	return (matches);
}


void	ARequest::findLocationBlock(RequestHandler& handler) // double check if this is entirely correct approach
{
	std::vector<std::string> uri_path_items;
	if (handler.header.redirected_path.empty())
		uri_path_items = splitPath(handler.header.path, '/');
	else
	{
		std::string temp = "/" + handler.header.redirected_path;
		uri_path_items = splitPath(temp, '/');
	}
	int	size = handler.server_config[0].locations.size();
	int	max = 0;

	for (int i = 0; i < size; i++)
	{
		std::vector<std::string> location_path_items = splitPath(handler.server_config[0].locations[i].path, '/');
		int matches = calcMatches(uri_path_items, location_path_items);
		if (matches > max)
		{
			handler.location_pos = i;
			max = matches;
		}
	}
}

int	ARequest::checkFileExistence(RequestHandler& handler)
{	
	if (handler.server_config[0].locations[handler.location_pos].path == "/") // maybe also cases where location ends with /? Is this possible?
		handler.header.redirected_path = handler.server_config[0].locations[handler.location_pos].root + handler.server_config[0].locations[handler.location_pos].path + handler.server_config[0].locations[handler.location_pos].index;
	else
		handler.header.redirected_path = handler.server_config[0].locations[handler.location_pos].root + handler.server_config[0].locations[handler.location_pos].path + "/" + handler.server_config[0].locations[handler.location_pos].index;

	std::cout << "index file path: " << handler.header.redirected_path << std::endl;
	int result = access(handler.header.redirected_path.c_str(), F_OK); // call to access allowed?
	std::cout << "file exists: " << result << std::endl;
	return (result);
}

bool	ARequest::checkFileType(RequestHandler& handler)
{
	// what if there are two dots in the path? // is there a better way to identify the file type requested?
	std::size_t found = handler.header.path.find('.');
	if (found == std::string::npos)
	{
		handler.file_type = "";
		return (0);
	}
	else
	{
		handler.file_type = handler.header.path.substr(found + 1);
		return (1);
	}
}

ARequest* ARequest::newRequest(RequestHandler& handler)
{
	// find server block if there are multiple that match
	if (handler.server_config.size() > 1)
		findServerBlock(handler);

	// find location block within server block if multiple exist
	if (handler.server_config[0].locations.size() > 1)
		findLocationBlock(handler);

	// check for redirect url within location block
	if (!(handler.server_config[0].locations[handler.location_pos].redirect.empty()))
		; // location found // set redirect url somewhere(?)
	else
	{
		// if the request is not for a file (otherwise the location has already been found) // probably create a function for that
		if (!checkFileType(handler))
		{
			// check if file constructed from root, location path and index exists
			// what if more than one index is specified?
			if (checkFileExistence(handler) == 0)
			{
				// if file does exist, search again for correct location
				findLocationBlock(handler);
				handler.file_path = handler.header.redirected_path;
				handler.file_type = handler.file_path.substr(handler.file_path.find('.') + 1); // create a function for that in case it is not a file type
			}
			else
			{
				// check if auto-index is on // only for GET request?
				// else
					; // what to do if index file does not exists and auto-index is not on? --> file not found error? for POST, GET and DELETE?
			}
		}
		else
		{
			handler.file_path = handler.server_config[0].locations[handler.location_pos].root + "/" + handler.header.path;
		}
	}

	// probably need to construct full URI here?

	std::cout << "location selected: " << handler.location_pos << std::endl;

	// check allowed methods for the selected location
	
	
	// some more error handling could go here if not already done in Request Handler (or move it here; e.g. check for http version)
	
	if (handler.header.method == "GET")
		return (new GETRequest(handler));
	else if (handler.header.method == "DELETE")
		;///
	else if (handler.header.method == "POST")
		;///
	// check if something else and thus not implemented; but currently alsready done when parsing
	return (NULL);
}
