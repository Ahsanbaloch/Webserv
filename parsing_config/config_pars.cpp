/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 15:08:27 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/04 16:37:41 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_pars.hpp"

config_pars::config_pars(char *argv)
{
	std::string config_file;
	std::string fileContent;
	readconfig(argv, fileContent);
	parse_server(fileContent);
	
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
	std::vector<std::string> serverBlocks;

	// split the server blocks
	//extractServerBlocks(serverBlocks, server_config); // to be implemented
}

void config_pars::extractServerBlocks(std::vector<std::string> &serverBlocks, std::string &server_config)
{
	size_t start = 0;
	size_t end = 0;
	//checkBasicServerBlockIntegrity(server_config);
	/* while ((end = server_config.find("server", start)) != std::string::npos)
	{
		serverBlocks.push_back(server_config.substr(start, end - start));
		start = end + 1;
	} */
}
