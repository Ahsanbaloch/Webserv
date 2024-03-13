/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 15:08:27 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/13 21:34:47 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_pars.hpp"
/* #include <set>
#include <utility> */


config_pars::config_pars(int argc, char **argv)
{
	std::string fileContent;
    std::string argument_value;
    argumentCheck(argc, argv, argument_value);
	readconfig(argument_value, fileContent);
    checkConsecutiveSameBraces(fileContent);
	parse_server_configs(fileContent);
	
}
config_pars::~config_pars() {}


void config_pars::argumentCheck(int argc, char **argv, std::string &argument_value)
{
    if (argc == 1)
        argument_value = "simple.conf";
    else if (argc == 2)
    {
        argument_value = argv[1];
        if (argument_value.find(".conf") == std::string::npos)
            throw InvalidConfigFileException();
    }
    else
        throw InvalidConfigFileException();
}

std::map<int, std::map<std::string, t_server_config> >&	config_pars::getConfigMap()
{
    return _configMap;
}

std::vector<t_server_config> &config_pars::getServerConfigsVector()
{
    return _server_configs_vector;
}

void config_pars::readconfig(std::string &argv, std::string &fileConetnt)
{
	std::ifstream file(argv);
	if (file.is_open())
	{
        //std::cout << "Reading config file: " << argv <<  std::endl;
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

void config_pars::parse_server_configs(std::string &server_config)
{
	std::vector<std::string> server_block;
	extractServer(server_block, server_config);
    //std::set<std::pair<int, std::string> > serverSet;
	for (size_t i = 0; i < server_block.size(); i++)
	{
        /* In many server configurations, each server is identified by a unique combination of IP address (or hostname) and port number. 
        This unique combination is often referred to as a socket. If two servers were allowed to have the same socket, 
        it would be impossible to determine which server should handle a given request. */

        
		t_server_config default_server_config;
        parse_server_block(default_server_config, server_block[i]);
        //std::cout << "Hello world\n" << std::endl;
        _server_configs_vector.push_back(default_server_config);
       /* check that the server name and port are unique */  
       /*std::pair<int,std::string> id(default_server_config.port, default_server_config.serverName);
        if (serverSet.count(id) != 0)
            throw DuplicateServerNameException();
        else
            serverSet.insert(id); */
        //_server_configs_vector[i] = default_server_config;
    
       /*  default_server_config.serverName = "default_server";
       if (!_configMap.count(default_server_config.port))
            _configMap[default_server_config.port][default_server_config.serverName] = default_server_config;
        t_server_config server_config;
        parse_server_block(server_config, server_block[i]);
        if (_configMap[server_config.port].count(server_config.serverName) != 0)
            throw DuplicateServerNameException();
        else
            _configMap[server_config.port][server_config.serverName] = server_config; */
	}
}

void config_pars::parse_server_block(t_server_config &server_config, const std::string &server_block)
{
    server_config.serverName = extractServerName(server_block);
    server_config.port = extractListen(server_block);
    //std::cout << "PORT :" << server_config.port << std::endl;
    server_config.errorPage = extractErrorPage(server_block);
    server_config.bodySize = extractBodySize(server_block);
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
        throw MissingValueException("File_Size");
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
		parseLocationBlock(location_config, location_blocks[i]);
		if (server_config.locationMap.count(location_config.path) != 0)
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
	location_config.cgi_ex = extractVariables("cgi-ext",location_block);
    if (location_config.path == "/redir") //should use this condition to check if the path is redir or not?
        location_config.redirect = extractVariables("redirect_url",location_block);
    /* if (!location_config.redirect.empty())
        std::cout << "Location: " << location_config.redirect << std::endl; */
    location_config.root = extractVariables("root", location_block);
    location_config.index = extractVariables("index", location_block);
    location_config.allowedMethods = extractVariables("allow_methods",location_block);
    location_config.autoIndex = extractAutoIndex(location_block);
}

bool config_pars::extractAutoIndex(const std::string &location_block)
{
	if (location_block.find("autoIndex") == std::string::npos)
		throw MissingValueException("autoIndex");
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

std::string config_pars::extractVariables(const std::string &variable, const std::string &location_block)
{
	if (location_block.find(variable) == std::string::npos)
		throw MissingValueException(variable);
	size_t start = location_block.find(variable) + variable.size();
	size_t end = location_block.find(";", start);
    if (location_block.find('\n', start) < location_block.find(';', start))
        throw MissingSemicolonException();
	std::string value = location_block.substr(start, end - start);
	removeLeadingWhitespaces(value);
	if (location_block.empty())
		throw MissingValueException(variable);
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

std::string config_pars::extractErrorPage(const std::string &server_block)
{
    size_t start = 0;
    size_t end = 0;
    std::string error_page;
    if ((start = server_block.find("error_page", start)) != std::string::npos)
    {
        start = skipWhitespace(server_block, start + 11);
        end = server_block.find(";", start);
        if (server_block.find('\n', start) < server_block.find(';', start))
            throw MissingSemicolonException();
        error_page = server_block.substr(start, end - start);
    }
    else
       throw MissingValueException("error_page");
    return error_page;
}

int config_pars::extractListen(const std::string &server_block)
{
    size_t start = 0;
    size_t end = 0;
    int port = -1;
    std::string listen;
    if ((start = server_block.find("listen", start)) != std::string::npos)
    {
        start = skipWhitespace(server_block, start + 6);
        end = server_block.find(";", start);
        if (server_block.find('\n', start) < server_block.find(';', start))
            throw MissingSemicolonException();
        listen = server_block.substr(start, end - start);
        port = checkHostPort(listen);
        if (port < 0 || port > 65535)
            throw InvalidPortException();
    }
    else
       throw MissingValueException("listen");
    return (port);
}


std::string config_pars::extractServerName(const std::string &server_block)
{
    size_t start = 0;
    size_t end = 0;
    std::string serverName;
    if ((start = server_block.find("Server_name", start)) != std::string::npos)
    {
        start = skipWhitespace(server_block, start + 11);
        end = server_block.find(";", start);
        if (server_block.find('\n', start) < server_block.find(';', start))
            throw MissingSemicolonException();
        serverName = server_block.substr(start, end - start);
    }
    else
        throw MissingValueException("Server Name");
    return serverName;
}

// The main function to extract server blocks
void config_pars::extractServer(std::vector<std::string> &serverblocks, const std::string &raw_data)
{
    size_t start = 0;
    size_t end = 0;
    size_t serverNum = 0;
    //checkConsecutiveSameBraces(raw_data);
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
        //std::cout << "Server Block: " << serverBlock.begin() << std::endl;
        serverNum++;
        if (serverNum >= 10)
            throw ExceededMaxServerNumberException();

        // Update start to be end for the next iteration
        start = end;
    }
    if (serverblocks.empty())
        throw EmptyConfigFileException();
}