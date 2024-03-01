/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_other_pars.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 19:20:52 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/01 18:29:18 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_other_pars.hpp"

server_other_pars::server_other_pars(/* args */)
{
}

server_other_pars::~server_other_pars()
{
}

void server_other_pars::pars_server_name(std::vector<std::string> servers)
{
    //std::cout << "Hello World" << std::endl;   
    for (size_t i = 0; i < servers.size(); i++)
    {
        //Parse each server block
        size_t pos = servers[i].find("server_name");
        if (pos != std::string::npos)
        {
            pos = servers[i].find_first_not_of(" \t", pos + 11);
            size_t end = servers[i].find(";", pos);
            server_name.push_back(servers[i].substr(pos, end - pos));
        }
    }
}


void server_other_pars::pars_root(std::vector<std::string> servers)
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        //Parse each server block
        size_t pos = servers[i].find("root");
        if (pos != std::string::npos)
        {
            pos = servers[i].find_first_not_of(" \t", pos + 4);
            size_t end = servers[i].find(";", pos);
            root.push_back(servers[i].substr(pos, end - pos));
        }
    }
}

void server_other_pars::pars_index(std::vector<std::string> servers)
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        //Parse each server block
        size_t pos = servers[i].find("index");
        if (pos != std::string::npos)
        {
            pos = servers[i].find_first_not_of(" \t", pos + 5);
            size_t end = servers[i].find(";", pos);
            index.push_back(servers[i].substr(pos, end - pos));
        }
    }
}

void server_other_pars::pars_error_page(std::vector<std::string> servers)
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        //Parse each server block
        size_t pos = servers[i].find("error_page");
        if (pos != std::string::npos)
        {
            pos = servers[i].find_first_not_of(" \t", pos + 10);
            size_t end = servers[i].find(";", pos);
            error_page.push_back(servers[i].substr(pos, end - pos));
        }
    }
}

void server_other_pars::pars_listen(std::vector<std::string> servers)
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        //Parse each server block
        size_t pos = servers[i].find("listen");
        if (pos != std::string::npos)
        {
            pos = servers[i].find_first_not_of(" \t", pos + 6);
            size_t end = servers[i].find(";", pos);
            listen.push_back(servers[i].substr(pos, end - pos));
        }
    }
}

void server_other_pars::pars_max_size_client_body(std::vector<std::string> servers)
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        //Parse each server block
        size_t pos = servers[i].find("max_size_client_body");
        if (pos != std::string::npos)
        {
            pos = servers[i].find_first_not_of(" \t", pos + 20);
            size_t end = servers[i].find(";", pos);
            max_size_client_body.push_back(servers[i].substr(pos, end - pos));
        }
    }
}


