/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_block_pars.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 11:40:57 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/01 18:24:25 by ahsalam          ###   ########.fr       */
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

std::vector<std::string> server_block_pars::outside_location(std::vector<std::string> servers)
{
    std::vector<std::string> outside_block;
    for (size_t i = 0; i < servers.size(); i++)
    {
        std::string outside;
        size_t pos = 0;
        while (pos < servers[i].size())
        {
            size_t start = servers[i].find("location", pos);
            if (start == std::string::npos)
            {
                    outside += servers[i].substr(pos);
                    break;
            }
            else
            {
                outside += servers[i].substr(pos, start - pos);

                size_t openBrackets = 0;
                start = servers[i].find('{', start);
                do
                {
                    if (servers[i][start] == '{')
                        ++openBrackets;
                    else if (servers[i][start] == '}')
                        --openBrackets;
                    ++start;
                }
            while (start < servers[i].size() && openBrackets > 0);

            pos = start;
            }
        }
            outside_block.push_back(outside);
    }
    return outside_block;
}
