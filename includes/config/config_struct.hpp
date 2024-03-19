/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_struct.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 14:14:51 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/19 13:37:16 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_STRUCT_HPP
#define CONFIG_STRUCT_HPP

#include <string>
#include <map>
#include <vector>

/* enum ErrorCode {
    PORT = 1,
    SERVERNAME,
    ERRORDIR,
    PATH,
    ROOT,
    INDEX,
    CGI,
    UPLOAD,
    REDIRECT,
    METHODS,
    AUTOINDEX,
	BODY_SIZE
}; */

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
	t_location_config							location; // remove it and test it
}	t_server_config;

#endif
