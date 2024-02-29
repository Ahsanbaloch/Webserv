/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_block_pars.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 11:37:09 by ahsalam           #+#    #+#             */
/*   Updated: 2024/02/29 19:26:10 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_BLOCK_PARS_HPP
#define SERVER_BLOCK_PARS_HPP

#include "config.hpp"
#include <map>

class server_block_pars
{
    private:
    /* data */
    public:
    std::string listen;
   /*  std::string server_name;
    std::string root;
    std::string index;
    std::string error_page;

    //Accessors
    std::string get_listen() const { return listen; };
    std::string get_Index() const { return index; };
    std::string get_server_name() const { return server_name; };
    std::string get_root() const { return root; };
    std::string get_error_page() const { return error_page; };

    //Mutators
    void set_listen(std::string listen) { this->listen = listen; };
    void set_Index(std::string index) { this->index = index; };
    void set_server_name(std::string server_name) { this->server_name = server_name; };
    void set_root(std::string root) { this->root = root; };
    void set_error_page(std::string error_page) { this->error_page = error_page; }; */


    server_block_pars(/* args */);
    ~server_block_pars();
    std::vector<std::string> server_pars_location(std::vector<std::string> servers); // Removed 'server_block_pars::'
    //std::map<std::string, std::string> location_pars(std::vector<std::string> locations);
    //std::vector<std::string> server_pars_others(std::vector<std::string> servers);

};


#endif