/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 15:08:27 by ahsalam           #+#    #+#             */
/*   Updated: 2024/04/02 12:42:51 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/config_pars.hpp"

config_pars::config_pars(int argc, char **argv) : _server_root(""), _server_index(""), _error_string("")
{
	std::string fileContent;
	std::string argument_value;
	argumentCheck(argc, argv, argument_value);
	readconfig(argument_value, fileContent);
	parse_server_configs(fileContent);
}

config_pars::~config_pars() {}

void	config_pars::argumentCheck(int argc, char **argv, std::string &argument_value)
{
	if (argc == 1)
		argument_value = "./config_files/simple.conf";
	else if (argc == 2)
	{
		argument_value = argv[1];
		if (argument_value.find(".conf") == std::string::npos)
			throw InvalidConfigFileException();
	}
	else
		throw InvalidConfigFileException();
}

std::map<std::string, std::vector<t_server_config> > &config_pars::getServerConfigsMap()
{
	return (_server_configs_map);
}

void	config_pars::readconfig(std::string &argv, std::string &fileConetnt)
{
	std::ifstream file(argv);
	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			fileConetnt += line;
			fileConetnt += "\n";
		}
		if (fileConetnt.empty())
		{
			std::cerr << "Error: Empty file" << std::endl;
			file.close();
			exit(1);
		}
		file.close();
	}
	else
		throw CantOpenConfigException();
}

void	config_pars::parse_server_configs(std::string &server_config)
{
	std::vector<std::string> server_block;
	extractServer(server_block, server_config);
	std::string ipPort;
	for (size_t i = 0; i < server_block.size(); i++)
	{   
		t_server_config default_server_config;
		ipPort.clear();
		parse_server_block(default_server_config, server_block[i]);
		std::stringstream port;
		port << default_server_config.port;
		ipPort.append(default_server_config.Ip + ":" +  port.str());
		if (_server_configs_map.count(ipPort))
		{
			bool isDuplicate = false;
			for (std::vector<t_server_config>::iterator it = _server_configs_map[ipPort].begin(); it != _server_configs_map[ipPort].end(); it++)
			{
				if (it->serverName == default_server_config.serverName)
				{
					isDuplicate = true;
					break;
				}
			}
			if (!isDuplicate)
				_server_configs_map[ipPort].push_back(default_server_config);
		}
		else
		{
			std::vector<t_server_config> temp;
			temp.push_back(default_server_config);
			_server_configs_map.insert(std::pair<std::string, std::vector<t_server_config> >(ipPort, temp));
		}
		checkDuplicatePath(_server_configs_map);
	}
}

void	config_pars::parse_server_block(t_server_config &server_config, const std::string &server_block)
{
	std::size_t locPos = server_block.find("location");
	std::string globalPart = server_block.substr(0, locPos);
	std::string locationPart = locPos != std::string::npos ? server_block.substr(locPos) : "";
	_server_index = extractServerVariable("index", globalPart);
	_server_root = extractServerVariable("root", globalPart);
	extractIpPort(globalPart, server_config.Ip, server_config.port);
	server_config.serverName = extractServerVariable("Server_name", globalPart);
	server_config.bodySize = extractBodySize(globalPart);
	_error_string = extractServerVariable("error_page", globalPart);
	if (!locationPart.empty())
    	Location_block(server_config, locationPart);
	else
		throw InvalidLocationException();
}

void config_pars::extractErrorPage(int &status, std::string & page, std::string split_string)
{
	if (!split_string.empty())
	{
		if (split_string.find(" ") != std::string::npos)
		{
			std::istringstream iss(split_string);
			if (!(iss >> status))
				throw MissingValueException("Error Page Status...");
			if (!(iss >> page))
				throw MissingValueException("Error Page value...");
		}
		else
			throw MissingValueException("InValid Error Page data");
	}
	else
		status = -1;
		page = "";
}

std::string config_pars::extractServerVariable(const std::string variable, const std::string &server_block)
{
	std::string value = "";
	if (server_block.find(variable) != std::string::npos)
	{
		size_t start = server_block.find(variable) + variable.size();
		size_t end = server_block.find(";", start);
		if (server_block.find('\n', start) < server_block.find(';', start))
			throw MissingSemicolonException();
		value = server_block.substr(start, end - start);
		removeLeadingWhitespaces(value);
		if (value.empty())
			throw MissingValueException(variable);
	}
	if (server_block.find("Server_name") == std::string::npos)
		return "default_server";
	return (value);
}

void	config_pars::extractIpPort(const std::string &server_block, std::string &ip, int &port)
{
	size_t start = 0;
	size_t end = 0;
	port = 9999;
	ip = "127.0.0.1";
	if ((start = server_block.find("listen", start)) != std::string::npos)
	{
		start = skipWhitespace(server_block, start + 6);
		end = server_block.find(";", start);
		if (server_block.find('\n', start) < server_block.find(';', start))
			throw MissingSemicolonException();
		const std::string ipPort = server_block.substr(start, end - start);
		if (ipPort.find(':') == std::string::npos)
			onlyIpPort(ipPort, port);
		checkIpPort(ipPort, ip, port);
	}
	else
		return ;
}

int	config_pars::extractBodySize(const std::string &server_block)
{
	size_t start = 0;
	size_t end = 0;
	int bodySize = -1;
	if ((start = server_block.find("client_max_body_size", start)) != std::string::npos)
	{
		start = skipWhitespace(server_block, start + 20);
		end = server_block.find(";", start);
		if (server_block.find('\n', start) < server_block.find(';', start))
			throw MissingSemicolonException();
		std::string value = server_block.substr(start, end - start);
		removeLeadingWhitespaces(value);
		if (value.empty())
			throw MissingValueException("File_Size");
		std::istringstream ss(value);
		if (ss >> bodySize)
			return bodySize;
		else
			throw InvalidbodySizeException();
	}
	else
		bodySize = 10000;
	return bodySize;
}


void	config_pars::Location_block(t_server_config &server_config, const std::string &config_block)
{
	std::vector<std::string> location_blocks;
	splitLocationBlocks(location_blocks, config_block);
	t_location_config location_config;
	if (location_blocks.empty())
		throw InvalidLocationException();
	for (size_t i = 0; i < location_blocks.size(); i++)
	{
		parseLocationBlock(location_config, location_blocks[i]);
		server_config.locations.push_back(location_config);
	}
}

void	config_pars::checkDuplicatePath(std::map<std::string, std::vector<t_server_config> > _server_configs_map)
{
	for (std::map<std::string, std::vector<t_server_config> >::iterator it = _server_configs_map.begin(); it != _server_configs_map.end(); ++it)
	{
		for (std::vector<t_server_config>::iterator vecIt = it->second.begin(); vecIt != it->second.end(); ++vecIt)
		{
			std::set<std::string> paths;
			for (std::vector<t_location_config>::iterator locIt = vecIt->locations.begin(); locIt != vecIt->locations.end(); ++locIt)
			{
				if (paths.count(locIt->path) > 0)
					throw DuplicateLocationNameException();
				paths.insert(locIt->path);
			}
		}
	}
}

void	config_pars::parseLocationBlock(t_location_config &location_config, const std::string &location_block)
{
	location_config.path = extractPath(location_block);
	location_config.redirect = extractVariables("redirect_url",location_block);
	if (location_config.path == "/cgi-bin" )
	{
		location_config.cgi_ex = extractVariables("cgi-ext",location_block);
		if (location_config.cgi_ex.empty())
			throw MissingValueException("cgi-ext");
	}
	location_config.root = extractVariables("root", location_block);
	location_config.index = extractVariables("index", location_block);
	/* if (_server_root.empty() && location_config.root.empty()) //recheck if the seg fault comes again
		throw MissingValueException("root"); */
	if (location_config.root.empty())
	{
		if (_server_root.empty())
		{
			//printf("Hello\n");
			throw MissingValueException("Root");
		}
		else
			location_config.root = _server_root;
	}
	if (_server_index.empty() && location_config.index.empty())
		location_config.index = "index.html";
	else if (location_config.index.empty())
		location_config.index = _server_index;
	std::string error_page_string = extractVariables("error_page", location_block);
	if (error_page_string.empty() && _error_string.empty())
	{
		extractErrorPage(location_config.errorPage.error_page_status, location_config.errorPage.html_page, "");
	}
	else if (!error_page_string.empty())
		extractErrorPage(location_config.errorPage.error_page_status, location_config.errorPage.html_page, error_page_string);
	else
		extractErrorPage(location_config.errorPage.error_page_status, location_config.errorPage.html_page, _error_string);
	
	allowMethods(location_config.GET, location_config.POST, location_config.DELETE, location_block);
	location_config.autoIndex = extractAutoIndex(location_block);
}

void	config_pars::allowMethods(bool &GET, bool &POST, bool &DELETE, const std::string location_block)
{
	GET = true;
	POST = false;
	DELETE = false;
	size_t start = 0;
	size_t end = 0;
	std::string methods;
	if ((start = location_block.find("allow_methods", start)) != std::string::npos)
	{
		start = skipWhitespace(location_block, start + 13);
		end = location_block.find(";", start);
		if (location_block.find('\n', start) < location_block.find(';', start))
			throw MissingSemicolonException();
		methods = location_block.substr(start, end - start);
		if (methods.empty())
			return ;
		if (methods.find("GET") != std::string::npos)
			GET = true;
		if (methods.find("POST") != std::string::npos)
			POST = true;
		if (methods.find("DELETE") != std::string::npos)
			DELETE = true;
	}
}

bool	config_pars::extractAutoIndex(const std::string &location_block)
{
	if (location_block.find("autoIndex") == std::string::npos)
		return false;
	size_t start = location_block.find("autoIndex") + 10;
	size_t end = location_block.find(";", start);
    if (location_block.find('\n', start) < location_block.find(';', start))
            throw MissingSemicolonException();
	std::string value = location_block.substr(start, end - start);
	removeLeadingWhitespaces(value);
    if (value.empty())
        value = "off";
	else if (value == "on")
		return true;
	else if (value == "off")
		return false;
	else
		throw MissingValueException("autoIndex");
    return false;
}

std::string	config_pars::extractVariables(const std::string &variable, const std::string &location_block)
{
	std::string value = "";
	if (location_block.find(variable) != std::string::npos)
	{
		size_t start = location_block.find(variable) + variable.size();
		size_t end = location_block.find(";", start);
    	if ((location_block.find('\n', start)) < (location_block.find(';', start)))
    	    throw MissingSemicolonException();
		value = location_block.substr(start, end - start);
		removeLeadingWhitespaces(value);
    	if (variable == "redirect_url" && (value.empty() || location_block.empty()))
    	    throw MissingValueException(variable);
	}
	return (value);
}

std::string	config_pars::extractPath(const std::string &location_block)
{
	if (location_block.find("location") == std::string::npos)
		throw InvalidPathException();
	size_t start = location_block.find("location") + 9;
	size_t end = location_block.find("{", start);
	std::string value = location_block.substr(start, end - start - 1);
	removeLeadingWhitespaces(value);
	if (value.empty())
		throw InvalidPathException();
	return (value);
}

void	config_pars::splitLocationBlocks(std::vector<std::string> &location_blocks, const std::string &server_block)
{
	size_t end = 0;
	size_t locationNum = 0;
	size_t start = server_block.find("location", end);
	while (start < server_block.length() && end < server_block.length())
	{
		if (server_block.find('}', start) == std::string::npos || server_block.find('}', start) > server_block.find("location", start + 9))
			throw InvalidLocationException();
		end = server_block.find('}', start);
		location_blocks.push_back(server_block.substr(start, end - start + 1));
		start = server_block.find("location", end);
		if (locationNum >= 10)
			throw InvalidLocationException();
		locationNum++;
	}
}

// The main function to extract server blocks
void	config_pars::extractServer(std::vector<std::string> &serverblocks, const std::string &raw_data)
{
	size_t start = 0;
	size_t end = 0;
	size_t serverNum = 0;
	checkConsecutiveSameBraces(raw_data);
	while ((start = raw_data.find("server", start)) != std::string::npos)
	{
		// Skip whitespace
		size_t temp = skipWhitespace(raw_data, start + 6);
		// Ensure the next character is an open brace
		if (raw_data[temp] != '{')
			throw MissingBracketException();
		// Find the end of the server block
		int braceCount = 1; // Count the number of open braces
		end = findServerBlockEnd(raw_data, temp, braceCount);
		if (braceCount > 0)
			throw MissingBracketException();
		// Extract the server block
		std::string serverBlock = raw_data.substr(start, end - start);
		if (serverBlock.empty())
			throw EmptyServerBlockException();
		serverblocks.push_back(serverBlock);
		serverNum++;
		if (serverNum >= 10)
			throw ExceededMaxServerNumberException();
		start = end;
	}
	if (serverblocks.empty())
		throw EmptyConfigFileException();
}
