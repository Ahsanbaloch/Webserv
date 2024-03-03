/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 16:41:19 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/03 15:10:29 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

config::config(char *config)
{
    file_operation(config);
}

config::~config()
{
}

int config::file_operation(char *config)
{
    if (file_read(config))
    {
        std::cerr << "Error: Could not read file " << config << std::endl;
        exit(1);
    }
    if (file_parsing())
    {
        std::cerr << "Error: Could not parse file " << config << std::endl;
        exit(1);
    }
    return 0;
}

int config::file_read(char *config)
{
    std::ifstream file(config);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << config << std::endl;
        return 1;
    }
    std::string line;
    while (std::getline(file, line))
    {
        content += line + "\n";
    }
       file.close();
    return 0;
}

int config::file_parsing() //separating the server blocks and storing them in a map 
{
    size_t start = 0;
    size_t end = 0;
    int server_id = 0;
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
        if (bracketcount != 0)
        {
            std::cerr << "Error: " << "bracket count is not 0" << std::endl;
            return 1;
        }
        config_map[server_id].push_back(content.substr(start, end - start + 1));
        end++;
        //config_map[server_id] = server;
        ++server_id;
    }
    return 0;
}

std::map<int, std::vector<std::string> > config::get_config_map()
{
    return config_map;
}