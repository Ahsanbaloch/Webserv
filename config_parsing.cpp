/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parsing.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 20:02:53 by ahsalam           #+#    #+#             */
/*   Updated: 2024/02/24 20:55:27 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>

class config_parsing
{
private:
	std::string _config_file;
public:
	config_parsing(/* args */){};
	~config_parsing(){};
	std::string config(char *config_file);
};

std::string config_parsing::config(char *config_file)
{
	std::stringstream strBuff;
	std::ifstream file(config_file);
	std::string line;
	std::string content;
	if (!file.is_open())
	{
		std::cerr << "Error: " << config_file << strerror(errno) << std::endl;
		return "";
	}
	strBuff << file.rdbuf();
	content = strBuff.str();
	if (content.empty())
	{
		std::cerr << "Error: " << config_file << " is empty" << std::endl;
		return "";
	}
	file.clear();
	file.seekg(0, std::ios::beg);
	while (std::getline(file, line))
	{
		content += line + "\n";
	}
	file.close();
	std::cout << "Hello world" << std::endl;

	return content;
}



int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }
    config_parsing config;
	std::string conf = config.config(argv[1]);
	//std::cout << "conf: " << conf << std::endl;
    return 0;
}