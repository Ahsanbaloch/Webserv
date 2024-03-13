/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 17:10:22 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/13 19:40:54 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "config_pars.hpp"

int main(int argc, char **argv)
{
    try
    {
        config_pars config(argc, argv);
        for (std::vector<t_server_config>::iterator it = config.getServerConfigsVector().begin(); it != config.getServerConfigsVector().end(); it++)
        {
            //std::cout << "Hello world!" << std::endl;
            //std::cout << "Server name: " << it->serverName << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

