/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 15:08:27 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/05 14:42:45 by ahsalam          ###   ########.fr       */
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
	{
		std::cerr << "Error: Unable to open file" << std::endl;
		exit(1);
	}
}

void config_pars::parse_server_configs(std::string &server_config)
{
	std::vector<std::string> server_block;
	extractServer(server_block, server_config);
	for (size_t i = 0; i < server_block.size(); i++)
	{
		//t_server_config default_server;
		std::cout << server_block[i] << std::endl;
	}
}

void config_pars::extractServer(std::vector<std::string> &serverblocks, const std::string &raw_data)
{
    size_t start = 0, end;
    size_t serverNum = 0;
	/* 
    // Check for consecutive brackets with possible spaces, tabs, or line breaks in between
    std::string::size_type pos = 0;
    while (pos < raw_data.length()) {
        if (raw_data[pos] == '{' || raw_data[pos] == '}') {
            char bracket = raw_data[pos];
            pos++;
            // Skip spaces, tabs, and line breaks
            while (pos < raw_data.length() && 
                   (raw_data[pos] == ' ' || raw_data[pos] == '\t' || raw_data[pos] == '\n')) {
                pos++;
            }
            // Check for another bracket of the same kind
            if (pos < raw_data.length() && raw_data[pos] == bracket) {
                throw std::runtime_error("Found consecutive " + std::string(1, bracket) + " at position: " + std::to_string(pos));
            }
        } else {
            pos++;
        }
    }

    // ... existing code to process the "server" blocks ...
    size_t start = 0, end;
    size_t serverNum = 0;
    // ... rest of your code ...
}
	 */
    while ((start = raw_data.find("server", start)) != std::string::npos)
    {
		//checkServerBlockSeparators(raw_data, start, end); // TODO: implement this function
        // Move past "server"
        start += 6; // Length of "server"
        // Skip whitespace
        while (start < raw_data.length() && (raw_data[start] == ' ' || raw_data[start] == '\t' || raw_data[start] == '\n'))
            start++;
        // Ensure the next character is "{"
        if (raw_data[start] != '{')
        {
            start++;
            continue; // Not a server block, continue to next "server"
        }
        // Find the end of the server block
        int braceCount = 1; // Count the number of open braces
        end = start + 1;
		std::cout << "end: " << end << std::endl;
        while (end < raw_data.length())
        {
            if (raw_data[end] == '{')
                braceCount++;
            else if (raw_data[end] == '}')
                braceCount--;
            end++;
        }
        if (braceCount > 0)
            {
				std::cout << "Malformed server block1" << std::endl;
				start = end;
				continue;
				//break; // Malformed server block
				//return;
			}
		else if (braceCount < 0)
			{
				std::cout << "Malformed server block2" << std::endl;
				start = end;
				continue;
				//return;
				//break; // Malformed server block
			}
        // Extract the server block
        std::string serverBlock = raw_data.substr(start, end - start);
        if (serverBlock.empty())
            std::cout << "empty server block" << std::endl;    //throw ServerBlockSeparatorException();
        serverblocks.push_back(serverBlock);
        serverNum++;
        if (serverNum >= 10)
            std::cout << "Max capacity" << std::endl;    //throw NoServerBlockException();
    }
    if (serverblocks.empty())
        std::cout << "No server block" << std::endl;    //throw NoServerBlockException();
}