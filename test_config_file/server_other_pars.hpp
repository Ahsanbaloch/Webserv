/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_other_pars.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 19:15:35 by ahsalam           #+#    #+#             */
/*   Updated: 2024/02/29 21:28:36 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_OTHER_PARS_HPP
#define SERVER_OTHER_PARS_HPP

#include "config.hpp"
#include "server_block_pars.hpp"

class server_other_pars : public server_block_pars
{
    private:
    /* data */
    public:
    //std::string listen;
    std::vector<std::string> listen;
    std::string server_name;
    std::string root;
    std::string index;
    std::string error_page;

    //Accessors
    //std::string get_listen() const { return listen; };
    std::vector<std::string> get_listen() const { return listen; };
    std::string get_Index() const { return index; };
    std::string get_server_name() const { return server_name; };
    std::string get_root() const { return root; };
    std::string get_error_page() const { return error_page; };

    //Mutators
    //void set_listen(std::string listen) { this->listen = listen; };
    void set_listen(std::vector<std::string> listen) { this->listen = listen; };
    void set_Index(std::string index) { this->index = index; };
    void set_server_name(std::string server_name) { this->server_name = server_name; };
    void set_root(std::string root) { this->root = root; };
    void set_error_page(std::string error_page) { this->error_page = error_page; };

    void pars_server_name(std::vector<std::string> servers);
    void pars_listen(std::vector<std::string> servers);
    void pars_root(std::vector<std::string> servers);
    void pars_index(std::vector<std::string> servers);
    void pars_error_page(std::vector<std::string> servers);

    server_other_pars(/* args */);
    ~server_other_pars();

}; 

#endif