/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_struct.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 14:14:51 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/04 14:25:11 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_STRUCT_HPP
#define CONFIG_STRUCT_HPP

#include <string>
#include <map>

typedef struct s_location_config {
	std::string	root;
	std::string	index;
	std::string	cgi_path;
	std::string	cgi_ex;
	std::string	redirect;
	std::string	allowedMethods;
	bool		autoIndex;
}	t_location_config;

typedef struct s_server_config {
	int											port;
	std::string									serverName;
	std::string									errorDir;
	int 										bodySize;
	std::map<std::string, t_location_config>	locationMap;
}	t_server_config;

#endif