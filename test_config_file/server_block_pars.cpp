/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_block_pars.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 11:40:57 by ahsalam           #+#    #+#             */
/*   Updated: 2024/02/29 19:27:30 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_block_pars.hpp"

server_block_pars::server_block_pars(/* args */) 
{
}

server_block_pars::~server_block_pars()
{
}

std::vector<std::string> server_block_pars::server_pars_location(std::vector<std::string> servers)
{
    std::vector<std::string> server_blocks;


    for (size_t i = 0; i < servers.size(); i++)
    {
        //std::cout << "\033[1;33mServer Block " << i << "\033[0m" << std::endl;
        size_t start = 0;
        size_t end = 0;
        while ((start = servers[i].find("location", end)) != std::string::npos)
        {
            //std::cout << "Location found" << std::endl;
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
            server_blocks.push_back(servers[i].substr(start, end - start + 1));
            ++end;
        }
    }
    return server_blocks;
}
/* 
std::vector<std::string> server_block_pars::server_pars_others(std::vector<std::string> servers)
{
    std::vector<std::string> server_other_properties;
    for (size_t i = 0; i < servers.size(); i++)
    {
        //Parse each server block
        server_block_pars serverProperties;

        //Extract properties from the server block
        size_t pos = 0;
        //Extract 'listen' property
        pos = servers[i].find("listen");
        //std::cout << "Pos: " << pos << "npos" << std::string::npos << std::endl;
        if (pos != std::string::npos)
        {
            //std::cout << "Pos: " << servers[i].find_first_of(" \t;", pos) << std::endl;
            pos = servers[i].find_first_not_of(" \t", pos + 6); // Skip 'listen' keyword
            size_t end = servers[i].find_first_of("\n;", pos);
            serverProperties.set_listen(servers[i].substr(pos, end - pos));
        }
        if (!serverProperties.get_listen().empty())
            std::cout << "Listen: " << serverProperties.get_listen() << std::endl; 

        //Extract 'index' property
        pos = servers[i].find("index");
        if (pos != std::string::npos)
        {
            //std::cout << "Listen found" << std::endl;
            pos = servers[i].find_first_not_of(" \t", pos + 5); // Skip 'index' keyword
            size_t end = servers[i].find_first_of("\n;", pos);
            serverProperties.set_Index(servers[i].substr(pos, end - pos));
        }
        if (!serverProperties.get_Index().empty())
            std::cout << "Index: " << serverProperties.get_Index() << std::endl;
        
        //Extract 'error_page' property
        pos = servers[i].find("error_page");
        if (pos != std::string::npos)
        {
            pos = servers[i].find_first_not_of(" \t", pos + 10); // Skip 'error_page' keyword
            size_t end = servers[i].find_first_of("\n;", pos);
            serverProperties.set_error_page(servers[i].substr(pos, end - pos));
        }
        if (!serverProperties.get_error_page().empty())
            std::cout << "Error Page: " << serverProperties.get_error_page() << std::endl;

        //Extract 'server_name' property
        pos = servers[i].find("server_name");
        if (pos != std::string::npos)
        {
            pos = servers[i].find_first_not_of(" \t", pos + 11); // Skip 'server_name' keyword
            size_t end = servers[i].find_first_of("\n;", pos);
            serverProperties.set_server_name(servers[i].substr(pos, end - pos));
        }
        if (!serverProperties.get_server_name().empty())
            std::cout << "Server Name: " << serverProperties.get_server_name() << std::endl;
        //Add the extracted properties to the list
        server_other_properties.push_back(servers[i]);

    }
    for (size_t i = 0; i < server_other_properties.size(); i++)
    {
        std::cout << "Server Block " << i << std::endl;
        std::cout << server_other_properties[i] << std::endl;
    }
    
    return server_other_properties;
}
 */