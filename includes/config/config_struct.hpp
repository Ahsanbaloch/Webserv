/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_struct.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 14:14:51 by ahsalam           #+#    #+#             */
/*   Updated: 2024/04/28 17:41:42 by ahsalam          ###   ########.fr       */
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
	std::vector<std::string>					cgi_ext;
	std::string									redirect;
	std::string									uploadDir;
	bool										GET;
	bool										POST;
	bool										DELETE;
	bool										autoIndex;
	s_errorPage									errorPage;
}	t_location_config;

typedef struct s_server_config {

	int											port;
	int 										bodySize;
	int											timeout;
	std::string									Ip;
	std::string									serverName;
	std::vector<t_location_config>				locations;
	s_errorPage									errorPage;
}	t_server_config;

#endif
