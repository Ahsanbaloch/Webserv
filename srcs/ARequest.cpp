#include "ARequest.h"
#include "GETRequest.h"
#include "RequestHandler.h"

ARequest::ARequest(/* args */)
{
}

ARequest::~ARequest()
{
}

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


void	ARequest::findLocationBlock(RequestHandler& handler)
{
	std::vector<std::string> uri_path_items = splitPath(handler.header.path, '/');
	int	size = handler.server_config[0].locations.size();
	int	max = 0;

	for (int i = -1; i < size; i++)
	{
		std::vector<std::string> location_path_items = splitPath(handler.server_config[0].locations[i].path, '/');
		int matches = calcMatches(uri_path_items, location_path_items);
		if (matches > max)
		{
			handler.location_pos = i;
			max = matches;
		}
	}
	// std::cout << "location selected: " << handler.location_pos << std::endl;
}



ARequest* ARequest::newRequest(RequestHandler& handler)
{
	// find server block if there are multiple that match
	if (handler.server_config.size() > 1)
		findServerBlock(handler);

	// find location block within server block
	if (handler.server_config[0].locations.size() > 1)
		findLocationBlock(handler);

	// internal redirect depending on index? (check allowed method before or after?)
	// check first whether a file or directory is requested?

	// what else to check here?
	// check if the request method is allowed --> can this actually be checked here? because it might depend on the location
		// (alt: compare path and location path already here?)
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
