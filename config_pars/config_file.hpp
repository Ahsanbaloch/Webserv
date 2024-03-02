/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/01 21:13:53 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/02 14:03:00 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include <cstdlib>


class config_file
{
    private :
        std::string content;
    protected:
        std::vector<std::string> file_data;
    public:
        config_file(){};
        config_file(char *config_file);
        ~config_file();
        void file_read(char *config_file);

        void server(std::string fileData);

        virtual void pars_server() = 0;

        //std::vector<std::string> get_file_data() const { return file_data; }
        
};

#endif