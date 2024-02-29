/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 17:10:22 by ahsalam           #+#    #+#             */
/*   Updated: 2024/02/29 21:34:17 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include "server_block_pars.hpp"
#include "server_other_pars.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cerr << "Error: Usage: " << av[0] << " <config_file>" << std::endl;
        return 1;
    }
    config c;
    std::vector<std::string> servers = c.file_read(av[1]);
    server_block_pars serverProperties;


    std::vector<std::string> server_blocks = serverProperties.server_pars_location(servers);
   /*  for (size_t i = 0; i < server_blocks.size(); i++)
    {
        std::cout << "\033[1;33mServer Block " << i << "\033[0m" << std::endl;
        std::cout << server_blocks[i] << std::endl;
    } */

     server_other_pars serverOtherProperties;
    serverOtherProperties.pars_server_name(servers);
    //std::cout << "server_name : " << serverOtherProperties.get_server_name() << std::endl;
    serverOtherProperties.pars_listen(servers);
    for (size_t i = 0; i < serverOtherProperties.get_listen().size(); i++)
    {
        std::cout << "listen : " << serverOtherProperties.get_listen()[i] << std::endl;
    }

}