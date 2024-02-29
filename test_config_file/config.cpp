/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 17:03:52 by ahsalam           #+#    #+#             */
/*   Updated: 2024/02/28 16:20:28 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

config::config(/* args */) : servers()
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
	//std::vector<std::string> servers;
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

/* std::vector<std::string> config::location_parts(std::vector<std::string> servers)
{
	std::vector<std::string> locations;
	for (size_t i = 0; i < servers.size(); i++)
	{
		size_t start = 0;
		size_t end = 0;
		while ((start = servers[i].find("location", end)) != std::string::npos)
		{
			//std::cout << "server separation..." << std::endl;
			int bracketcount = 0;
			for (end = start; end < servers[i].size(); ++end)
			{
				if (servers[i][end] == '{')
					++bracketcount;
				else if (servers[i][end] == '}')
					--bracketcount;
				if (bracketcount == 0 && servers[i][end] == '}')
					break;
			}
			std::string location = servers[i].substr(start, end - start + 1);
			std::stringstream ss;
			ss << i;
			std::string server_Id = ss.str();
			locations.push_back(server_Id + ", " + location);
			//locations.push_back(servers[i].substr(start, end - start + 1));
			end++;
		}
	}
	return locations;
} */
