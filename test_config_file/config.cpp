/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 17:03:52 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/01 21:11:26 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"



/* void config::server(std::vector<std::string> fileData)
{
	size_t start = 0;
	size_t end = 0;

	while ((start = fileData[0].find("server", end)) != std::string::npos)
	{
		int bracketcount = 0;
		for (end = start; end < fileData[0].size(); ++end)
		{
			if (fileData[0][end] == '{')
				++bracketcount;
			else if (fileData[0][end] == '}')
				--bracketcount;
			if (bracketcount == 0 && fileData[0][end] == '}')
				break;
		}
		servers.push_back(fileData[0].substr(start, end - start + 1));
		end++;
	}
} */






/* config::config()
{
}

config::~config()
{
}

std::vector<std::string> config::file_read(char *config_file)
{
	std::stringstream strBuff;
	std::ifstream file(config_file);
	std::string line;
	std::string content;
	if (!file.is_open())
	{
		std::cerr << "Error: " << config_file << " " << strerror(errno) << std::endl;
		return std::vector<std::string>();
	}
	strBuff << file.rdbuf();
	content = strBuff.str();
	if (content.empty())
	{
		std::cerr << "Error: " << config_file << " is empty" << std::endl;
		return std::vector<std::string>();
	}
	 file.clear();
	file.seekg(0, std::ios::beg); 
	file.close();

	return server_parts(content);
}

std::vector<std::string> config::server_parts(std::string content)
{
	size_t start = 0;
	size_t end = 0;

	while ((start = content.find("server", end)) != std::string::npos)
	{ 
		int bracketcount = 0;
		for (end = start; end < content.size(); ++end)
		{
			if (content[end] == '{')
				++bracketcount;
			else if (content[end] == '}')
				--bracketcount;
			if (bracketcount == 0 && content[end] == '}')
				break;
		}
		servers.push_back(content.substr(start, end - start + 1));
		end++;
	}
	return servers;
}
 */