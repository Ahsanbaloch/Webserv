/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_struct.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 14:14:51 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/28 15:41:43 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_STRUCT_HPP
#define CONFIG_STRUCT_HPP

#include <string>
#include <map>
#include <vector>

typedef struct s_errorPage
{
	int											error_page_status;
	std::string									html_page;
}	t_errorPage;

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
	std::vector<t_location_config>				locations;
	s_errorPage									errorPage;
}	t_server_config;

#endif
