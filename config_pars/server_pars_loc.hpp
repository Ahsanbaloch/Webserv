/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_pars_loc.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 13:21:57 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/02 14:24:48 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_PARS_LOC_HPP
#define SERVER_PARS_LOC_HPP

#include "config_file.hpp"

class server_pars_loc : public config_file
{
    private:
      std::vector<std::string> location_parsing;
    public:
        server_pars_loc(/* args */);
        ~server_pars_loc();
        std::vector<std::string> get_server_location_data() const;
        void set_server_location_data(std::vector<std::string> location_parsing);
        void pars_server();

};


#endif