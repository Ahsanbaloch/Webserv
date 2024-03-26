/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_struct.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamesser <mamesser@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 14:14:51 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/26 11:14:26 by mamesser         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_STRUCT_HPP
#define CONFIG_STRUCT_HPP

#include <string>
#include <map>
#include <vector>

typedef struct s_location_config {
	std::string									path;
	std::string									root;
	std::string									index;
	std::string									cgi_ex;
	std::string									redirect;
	bool										GET;
	bool										POST;
	bool										DELETE;
	bool										autoIndex;
}	t_location_config;

typedef struct s_server_config {

	int											port;
	int 										bodySize;
	std::string									Ip;
	std::string									server_root;
	std::string									server_index;
	std::string									serverName;
	std::string									errorPage;
	std::vector<t_location_config>				locations;
}	t_server_config;

#endif
