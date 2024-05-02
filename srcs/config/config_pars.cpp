/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 15:08:27 by ahsalam           #+#    #+#             */
/*   Updated: 2024/04/28 17:46:15 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/config_pars.hpp"

config_pars::config_pars(int argc, char **argv) 
: _server_root(""), _server_index(""), _error_string(""), _upload(""), _GET(true), _POST(false), _DELETE(false)
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
	std::ifstream file(argv.c_str());
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
	_server_root = extractServerVariable("root", globalPart);
	_server_index = extractServerVariable("index", globalPart);
	extractIpPort(globalPart, server_config.Ip, server_config.port);
	extractTimeout(globalPart, server_config.timeout);
	server_config.serverName = extractServerVariable("Server_name", globalPart);
	server_config.bodySize = extractBodySize(globalPart);
	_error_string = extractServerVariable("error_page", globalPart);
	_upload	= extractServerVariable("uploadDir", globalPart);
	allowMethods(_GET, _POST, _DELETE,  globalPart);
	if (!locationPart.empty())
    	Location_block(server_config, locationPart);
	else
		throw InvalidLocationException();
}

void config_pars::extractTimeout(const std::string &server_block, int &timeout)
{
	size_t start = 0;
	size_t end = 0;
	timeout = 20;
	if ((start = server_block.find("timeout", start)) != std::string::npos)
	{
		start = skipWhitespace(server_block, start + 7);
		end = server_block.find(";", start);
		if (server_block.find('\n', start) < server_block.find(';', start))
			throw MissingSemicolonException();
		const std::string timeout_str = server_block.substr(start, end - start);
		std::istringstream iss(timeout_str);
		if (!(iss >> timeout))
			throw MissingValueException("timeout isn't int ");
		else
		{
			if (timeout > 60 || timeout < 10)
				timeout = 20;
		}
	}

}

void config_pars::extractErrorPage(int &status, std::string &page, std::string split_string, std::string merge_str)
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
			else
			{
 				page = merge_str + page;
				page = removeMultipleSlashes(page);
				removLSlashes(page);
			}
			if (access(page.c_str() , F_OK) != 0)
    		{
				status = -1;
				page = "";
			}
		}
		else
			throw MissingValueException("InValid Error Page data");
	}
	else
	{
		status = -1;
		page = "";
	}
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

std::string config_pars::parseRootPath(t_location_config & location_config, const std::string & location_block)
{
	std::string rootValue = extractVariables("root", location_block);
	rootValue = rootValueCheck(location_config, rootValue, _server_root);
	std::string path = extractPath(location_block);
	path = "/" + path;
	path = removeMultipleSlashes(path);
	location_config.path = path; //used this path
	rootValue = rootValue + "/" + path + "/";
	return (rootValue);
}

void config_pars::parseMethod(t_location_config &location_config, const std::string &location_block)
{
	if (allowMethods(location_config.GET, location_config.POST, location_config.DELETE, location_block))
	{
		location_config.GET = _GET;
		location_config.POST = _POST;
		location_config.DELETE = _DELETE;
	}
}

void config_pars::cgiOp(t_location_config &location_config, const std::string &location_block)
{
		location_config.cgi_ext.clear();
		std::string cgi_ext = extractVariables("cgi-ext",location_block);

		if (location_config.path == "/cgi-bin" && cgi_ext.empty())
			throw MissingValueException("cgi-ext");
		extractMultipleCgi(location_config.cgi_ext, cgi_ext);
}

void config_pars::uploadProcess(t_location_config &location_config, const std::string &location_block, std::string merge_string)
{
	std::string upload = extractVariables("uploadDir", location_block);
	if (upload.empty() && _upload.empty())
		location_config.uploadDir = "";
	else if (!upload.empty())
	{
		SlashConvert(upload, merge_string, location_config.uploadDir);
		if (access(location_config.uploadDir.c_str(), F_OK) != 0)
			location_config.uploadDir = ""; // how to create the directory or it should return empty?
	}
	else
	{
		SlashConvert(_upload, merge_string, location_config.uploadDir);
		if (access(location_config.uploadDir.c_str(), F_OK) != 0)
			location_config.uploadDir = ""; // how to create the directory or it should return empty?
	}
}

void	config_pars::parseLocationBlock(t_location_config &location_config, const std::string &location_block)
{
	std::string merge_string = parseRootPath(location_config, location_block);
	location_config.redirect = extractVariables("redirect_url",location_block);
	if (!location_config.redirect.empty() 
			&& location_config.redirect.substr(0,7) != "http://" 
			&& location_config.redirect.substr(0,8) != "https://")
		location_config.redirect = "http://" + location_config.redirect;
	parseMethod(location_config, location_block);
	location_config.autoIndex = extractAutoIndex(location_block);
	cgiOp(location_config, location_block);
	std::string index = merge_string + extractVariables("index", location_block);
	location_config.index = removeMultipleSlashes(index);
	if (_server_index.empty() && location_config.index.empty())
		location_config.index = merge_string + "index.html";
	else if (location_config.index.empty())
		SlashConvert(index, merge_string, location_config.index);
	uploadProcess(location_config, location_block, merge_string);
	std::string error_page_string = extractVariables("error_page", location_block); //TODO: make another function for readability Error page
	if (!error_page_string.empty())
		extractErrorPage(location_config.errorPage.error_page_status, location_config.errorPage.html_page, error_page_string, merge_string);
	else if (error_page_string.empty() && _error_string.empty())
		extractErrorPage(location_config.errorPage.error_page_status, location_config.errorPage.html_page, "", merge_string);
	else
		extractErrorPage(location_config.errorPage.error_page_status, location_config.errorPage.html_page, _error_string, merge_string);
}

int config_pars::allowMethods(bool &GET, bool &POST, bool &DELETE, const std::string location_block)
{

	size_t start = 0;
	size_t end = 0;
	std::string a_methods;
	if ((start = location_block.find("allow_methods", start)) != std::string::npos)
	{
		start = skipWhitespace(location_block, start + 13);
		end = location_block.find(";", start);
		if (location_block.find('\n', start) < location_block.find(';', start))
			throw MissingSemicolonException();
		a_methods = location_block.substr(start, end - start);
		if (a_methods.empty())
			return (1);
		if (a_methods.find("GET") != std::string::npos)
			GET = true;
		else
			GET = false;
		if (a_methods.find("POST") != std::string::npos)
			POST = true;
		else
			POST = false;
		if (a_methods.find("DELETE") != std::string::npos)
			DELETE = true;
		else
			DELETE = false;
	}
	else
		return (1);
	return (0);
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
		/* if (variable == "cgi-bin" && (value.empty() || location_block.empty()))
    	    throw MissingValueException(variable); */
		
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
	value = removeMultipleSlashes(value);
	if (value.length() > 1)
		removLSlashes(value);
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
