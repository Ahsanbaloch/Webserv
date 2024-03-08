/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 15:08:27 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/08 19:45:16 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_pars.hpp"


config_pars::config_pars(char *argv)
{
	std::string config_file;
	std::string fileContent;
	readconfig(argv, fileContent);
	parse_server_configs(fileContent);
	
}
config_pars::~config_pars()
{
}

std::map<int, std::map<std::string, t_server_config> >&	config_pars::getConfigMap()
{
    return _configMap;
}

void config_pars::readconfig(char *argv, std::string &fileConetnt)
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
        throw CantOpenConfigException(); //TODO: create this function
}

void config_pars::parse_server_configs(std::string &server_config)
{
	std::vector<std::string> server_block;
	extractServer(server_block, server_config);
	for (size_t i = 0; i < server_block.size(); i++)
	{
		t_server_config default_server_config;
        parse_server_block(default_server_config, server_block[i]);
        default_server_config.serverName = "default_server"; //understand this shit
        if (!_configMap.count(default_server_config.port))
            _configMap[default_server_config.port][default_server_config.serverName] = default_server_config;
        t_server_config server_config;
        parse_server_block(server_config, server_block[i]);
        if (_configMap[server_config.port].count(server_config.serverName) != 0)
            std::cout << "Duplicate server block" << std::endl;    //throw DuplicateServerBlockException();
        else
            _configMap[server_config.port][server_config.serverName] = server_config;
	}
}

void config_pars::parse_server_block(t_server_config &server_config, const std::string &server_block)
{
    server_config.serverName = extractServerName(SERVERNAME, server_block);
    server_config.port = extractListen(PORT, server_block); //convert the data from string to int
    //std::cout << "port: " << server_config.port << std::endl;
    server_config.errorPage = extractErrorPage(ERRORDIR, server_block);
    /* issue in body size... check it again*/
    server_config.bodySize = extractBodySize(server_block);
    //std::cout << "bodySize: " << server_config.bodySize << std::endl;
    if (server_config.port < 0 || server_config.port > 65535)
        throw InvalidPortException();
    Location_block(server_config, server_block);
}

int config_pars::extractBodySize(const std::string &server_block)
{
    size_t start = 0;
    size_t end = 0;
    int bodySize = -1;
    if ((start = server_block.find("client_max_body_size", start)) != std::string::npos)
    {
        start = skipWhitespace(server_block, start + 20);
        end = server_block.find(";", start);
        std::string value = server_block.substr(start, end - start);
        removeLeadingWhitespaces(value);
        if (value.empty())
            throw ValueMissingException();
        std::istringstream ss(value);
        if (ss >> bodySize)
            return bodySize;
        else
            throw InvalidbodySizeException();
    }
    else
        throw ValueMissingException();
    return bodySize;
}


void config_pars::Location_block(t_server_config &server_config, const std::string &server_block)
{
    std::vector<std::string> location_blocks;

    splitLocationBlocks(location_blocks, server_block);
	if (location_blocks.empty())
		throw InvalidLocationException();
	for (size_t i = 0; i < location_blocks.size(); i++)
	{
		t_location_config location_config;
		parseLocationBlock(location_config, location_blocks[i]); //TODO: create this function
		if (server_config.locationMap.count(location_config.path) != 0) //explanation from here
            throw InvalidPathException();
		else
			server_config.locationMap[location_config.path] = location_config;
	}
	if (server_config.locationMap.count("/") == 0)
		throw NoRootLocationException();
}


void config_pars::parseLocationBlock(t_location_config &location_config, const std::string &location_block)
{
    location_config.path = extractPath(location_block);
	//std::cout << "path: " << location_config.path << std::endl;
	location_config.cgi_ex = extractVariables("cgi-ext",location_block);
	location_config.redirect = extractVariables("redirect_url",location_block);
    location_config.root = extractVariables("root", location_block); //TODO: create this function
    location_config.index = extractVariables("index", location_block); //TODO: create this function
    location_config.allowedMethods = extractVariables("allow_methods",location_block); //TODO: create this function
    location_config.autoIndex = extractAutoIndex(location_block); //TODO: create this function
}

bool config_pars::extractAutoIndex(const std::string &location_block)
{
	if (location_block.find("autoIndex") == std::string::npos)
		throw ValueMissingException();
	size_t start = location_block.find("autoIndex") + 10;
	size_t end = location_block.find(";", start);
	std::string value = location_block.substr(start, end - start);
	removeLeadingWhitespaces(value);
	if (value.empty())
		throw ValueMissingException();
	if (value == "on")
		return true;
	else if (value == "off")
		return false;
	return false;
}

std::string config_pars::extractVariables(const std::string &variable, const std::string &location_block)
{
	if (location_block.find(variable) == std::string::npos)
		throw ValueMissingException();
	size_t start = location_block.find(variable) + variable.size();
	size_t end = location_block.find(";", start);
	std::string value = location_block.substr(start, end - start);
	removeLeadingWhitespaces(value);
	if (location_block.empty())
		throw ValueMissingException();
/* 	else if (!value.empty())
		std::cout << "value: " << value << std::endl; */
	return (value);
}

std::string config_pars::extractPath(const std::string &location_block)
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

void config_pars::splitLocationBlocks(std::vector<std::string> &location_blocks, const std::string &server_block)
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

std::string config_pars::extractErrorPage(int num, const std::string &server_block)
{
    size_t start = 0;
    size_t end = num; //change back to 0 instead of num
    std::string error_page;
    if ((start = server_block.find("error_page", start)) != std::string::npos)
    {
        start = skipWhitespace(server_block, start + 11);
        end = server_block.find(";", start);
        error_page = server_block.substr(start, end - start);
    }
    else
       throw ValueMissingException();
    return error_page;
}

int config_pars::extractListen(int num, const std::string &server_block)
{
    size_t start = 0;
    num = 0;
    size_t end = num; //change back to 0 instead of num
    int port = -1;
    std::string listen;
    if ((start = server_block.find("listen", start)) != std::string::npos)
    {
        start = skipWhitespace(server_block, start + 6);
        end = server_block.find(";", start);
        listen = server_block.substr(start, end - start);
        //std::cout << listen << std::endl;
        port = checkHostPort(listen);
    }
    else
       throw ValueMissingException();
    return (port);
}


std::string config_pars::extractServerName(int num, const std::string &server_block)
{
    size_t start = 0;
    size_t end = num; //change back to 0 instead of num
    std::string serverName;
    if ((start = server_block.find("server_name", start)) != std::string::npos)
    {
        start = skipWhitespace(server_block, start + 11);
        end = server_block.find(";", start);
        serverName = server_block.substr(start, end - start);
    }
    else
        throw ValueMissingException();    //give default name to server
    return serverName;
}

// The main function to extract server blocks
void config_pars::extractServer(std::vector<std::string> &serverblocks, const std::string &raw_data)
{
    size_t start = 0;
    size_t end = 0;
    size_t serverNum = 0;
    checkConsecutiveSameBraces(raw_data);
    while ((start = raw_data.find("server", start)) != std::string::npos)
    {
        // Do not move past "server"
        // Skip whitespace
        size_t temp = skipWhitespace(raw_data, start + 6); // Temporary variable to skip "server"
        // Ensure the next character is "{"
        if (raw_data[temp] != '{')
        {
            start++;
            continue; // Not a server block, continue to next "server"
        }
        // Find the end of the server block
        int braceCount = 1; // Count the number of open braces
        end = findServerBlockEnd(raw_data, temp, braceCount);
        if (braceCount > 0)
            throw MissingClosingBracketException();
        /* {
            std::cout << "Malformed server block1" << std::endl;
            start = end; // Move to the end of the malformed block
            continue;
        } */
        // Extract the server block
        std::string serverBlock = raw_data.substr(start, end - start);
        if (serverBlock.empty())
            throw EmptyServerBlockException();
        serverblocks.push_back(serverBlock);
        serverNum++;
        if (serverNum >= 10)
            throw ExceededMaxServerNumberException();

        // Update start to be end for the next iteration
        start = end;
    }
    if (serverblocks.empty())
        throw EmptyConfigFileException();
}