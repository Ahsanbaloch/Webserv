/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_parts.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 15:04:24 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/03 15:12:42 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_parts.hpp"

server_parts::server_parts()
{
	config config;
	std::map<int, std::vector<std::string> > config_map = config.get_config_map();
}
server_parts::~server_parts()
{
}

std::map<int, std::vector<std::string> > server_parts::get_location()
{
	size_t start = 0;
	size_t end = 0;
	int server_id = 0;

	/* 
	 int server_id = 0;
    for (std::map<int, std::vector<std::string> >::iterator it = config_map.begin(); it != config_map.end(); ++it)
    {
        for (std::vector<std::string>::iterator vec_it = it->second.begin(); vec_it != it->second.end(); ++vec_it)
        {
            // Process the server block string here
            // This will depend on the format of your server block strings
            std::string server_block = *vec_it;
        }
        ++server_id;
    }
	 */

	return _location;
}