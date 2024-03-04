/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 15:08:27 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/04 20:18:52 by ahsalam          ###   ########.fr       */
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
	while ((end = raw_data.find("}", start)) != std::string::npos)
	{
		//checkServerBlockSeparators(raw_data, start, end); // TODO: implement this function
		size_t serverStart = raw_data.find("server", start);
		if (serverStart == std::string::npos)
		{
			// Move to the next "{" and continue
			start = raw_data.find("{", start);
			if (start == std::string::npos)
				break; // No more server blocks
			continue;
		}
		// Move past "server"
		serverStart += 6; // Length of "server"
		while (serverStart < raw_data.length() && (raw_data[serverStart] == ' ' || raw_data[serverStart] == '\t' || raw_data[serverStart] == '\n'))
			serverStart++;
		// Ensure the next character is "{"
		if (raw_data[serverStart] == '{')
		{
			// Find the end of the server block
			end = raw_data.find("}", serverStart);
			if (end == std::string::npos)
				break; // Malformed server block
			// Extract the server block
			std::string serverBlock = raw_data.substr(serverStart, end - serverStart + 1);
			if (serverBlock.empty())
				std::cout << "empty server block" << std::endl;    //throw ServerBlockSeparatorException();
			serverblocks.push_back(serverBlock);
			serverNum++;

			start = end + 1; // Move past the "}" and continue
		}
		else
		{
			// Move to the next "{" and continue
			start = raw_data.find("{", start);
			if (start == std::string::npos)
				break; // No more server blocks
		}
		if (serverNum >= 10)
			std::cout << "Max capacity" << std::endl;    //throw NoServerBlockException();
	}
	if (serverblocks.empty())
		std::cout << "No server block" << std::endl;    //throw NoServerBlockException();
}

