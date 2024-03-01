/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_block_pars.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 11:37:09 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/01 16:51:09 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_BLOCK_PARS_HPP
#define SERVER_BLOCK_PARS_HPP

#include "config.hpp"
#include <map>

class server_block_pars
{
    private:
    
    public:
    server_block_pars(/* args */);
    ~server_block_pars();
    std::vector<std::string> server_pars_location(std::vector<std::string> servers); // Removed 'server_block_pars::'
    std::vector<std::string> outside_location(std::vector<std::string> servers);
};


#endif