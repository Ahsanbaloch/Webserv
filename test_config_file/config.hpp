/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 17:01:26 by ahsalam           #+#    #+#             */
/*   Updated: 2024/02/27 17:28:46 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>

class config
{

public:
    config(/* args */){};
    ~config(){};
    std::vector<std::string> file_read(char *config_file);
    std::vector<std::string> server_parts(std::string content);
    std::vector<std::string> location_parts(std::vector<std::string> servers);
};

#endif