/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 15:08:27 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/05 19:02:55 by ahsalam          ###   ########.fr       */
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
		std::cerr << "Error: Unable to open file" << std::endl; //throw std::runtime_error("Unable to open file");
		exit(1);
	}
}

void config_pars::parse_server_configs(std::string &server_config)
{
	std::vector<std::string> server_block;
	extractServer(server_block, server_config);
	for (size_t i = 0; i < server_block.size(); i++)
	{
		t_server_config default_server_config;
	}
}

void config_pars::extractServer(std::vector<std::string> &serverblocks, const std::string &raw_data)
{
    size_t start = 0;
    size_t end = 0;
    size_t serverNum = 0;
    checkConsecutiveSameBraces(raw_data);
    while ((start = raw_data.find("server", start)) != std::string::npos)
    {
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
            end = start + 1; // Reset end
            break;
        }
        else if (braceCount < 0)
        {
            std::cout << "Malformed server block2" << std::endl;
            end = start + 1; // Reset end
            break;
        }
        // Extract the server block
        std::string serverBlock = raw_data.substr(start, end - start);
        if (serverBlock.empty())
            std::cout << "empty server block" << std::endl;    //throw ServerBlockSeparatorException();
        serverblocks.push_back(serverBlock);
        serverNum++;
        if (serverNum >= 10)
            std::cout << "Max capacity" << std::endl;    //throw NoServerBlockException();

        // Update start to be end for the next iteration
        start = end;
    }
    if (serverblocks.empty())
        std::cout << "No server block" << std::endl;    //throw NoServerBlockException();
}

