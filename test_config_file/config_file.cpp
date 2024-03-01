/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/01 21:16:13 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/01 21:51:38 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_file.hpp"

config_file::config_file(char *config_file)
{
	file_read(config_file);
	server(content);

}

config_file::~config_file()
{}

void config_file::file_read(char *config_file)
{
	std::stringstream strBuff;
	std::ifstream file(config_file);
	std::string line;
	if (!file.is_open())
	{
		std::cerr << "Error: " << config_file << " " << strerror(errno) << std::endl;
		exit(1);
	}
	while (std::getline(file, line))
	{
		this->content += line + "\n";
	}
	file.close();
}

void config_file::server(std::string fileData)
{
	size_t start = 0;
	size_t end = 0;

	while ((start = fileData.find("server", end)) != std::string::npos)
	{
		int bracketcount = 0;
		for (end = start; end < fileData.size(); ++end)
		{
			if (fileData[end] == '{')
				++bracketcount;
			else if (fileData[end] == '}')
				--bracketcount;
			if (bracketcount == 0 && fileData[end] == '}')
				break;
		}
		file_data.push_back(fileData.substr(start, end - start + 1));
		end++;
	}
	for (size_t i = 0; i < file_data.size(); i++)
	{
		std::cout << file_data[i] << std::endl;
	}
}