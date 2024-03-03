/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 16:38:17 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/03 15:05:50 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP


#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <map>


class config
{
    private :
        std::string content;
    protected:
        std::vector<std::string> file_data;
        std::map<int, std::vector<std::string> > config_map;
        
    public:
        config(){};
        config(char *config);
        ~config();
        int file_operation(char *config);
        int file_read(char *config);
        int file_parsing();
        std::map<int, std::vector<std::string> > get_config_map();

};

#endif