/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_struct.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 14:14:51 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/17 14:49:24 by ahsalam          ###   ########.fr       */
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
	std::string	path;
	std::string	root;
	std::string	index;
	std::string	cgi_ex;
	std::string	redirect;
	std::string	allowedMethods;
	bool		autoIndex;
}	t_location_config;

typedef struct s_server_config {
	std::string									Ip;
	int											port;
	std::string									serverName;
	std::string									errorPage;
	int 										bodySize;
	std::vector<t_location_config>				locations;
	t_location_config							location;
}	t_server_config;

#endif
