/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 17:10:22 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/14 20:26:34 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "config_pars.hpp"

int main(int argc, char **argv)
{

   
    try
    {
        std::map<std::string, std::vector<t_server_config> > test;
        config_pars config(argc, argv);
       /*  for (std::vector<t_server_config>::iterator it = config.getServerConfigsVector().begin(); it != config.getServerConfigsVector().end(); it++)
        {
             std::string ipPort;
             ipPort.append(it->Ip + ":");
             ipPort.append(std::to_string(it->port));
             if (test.count(ipPort))
             {
                 test[ipPort].push_back(*it);
             }
             else
             {
                 std::vector<t_server_config> temp;
                 temp.push_back(*it);
                 test.insert(std::pair<std::string, std::vector<t_server_config> >(ipPort, temp));
             }
             //std::cout << "Hello world!" << std::endl;
             //std::cout << "Server name: " << it->serverName << std::endl;
        } */
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

