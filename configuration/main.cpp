/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 17:10:22 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/16 11:38:00 by ahsalam          ###   ########.fr       */
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

        std::map<std::string, std::vector<t_server_config> >& serverConfigsMap = config.getServerConfigsMap();
    //std::map<std::string, std::vector<t_server_config> >& serverConfigsMap = config.getServerConfigsMap();
for (std::map<std::string, std::vector<t_server_config> >::iterator it = serverConfigsMap.begin(); it != serverConfigsMap.end(); ++it)
{
    std::cout << "Key: " << it->first << ", Number of vectors: " << it->second.size() << std::endl;
    for (std::vector<t_server_config>::iterator vecIt = it->second.begin(); vecIt != it->second.end(); ++vecIt)
    {
        // Print the data of the server configuration here
        // For example, if t_server_config has members named 'Ip', 'port', and 'serverName', you could do:
        std::cout << "IP: " << vecIt->Ip << ", Port: " << vecIt->port << ", Server Name: " << vecIt->serverName << std::endl;
    }
}
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

