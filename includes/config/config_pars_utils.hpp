/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars_utils.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 16:51:42 by ahsalam           #+#    #+#             */
/*   Updated: 2024/04/21 19:18:22 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARS_UTILS_HPP
#define CONFIG_PARS_UTILS_HPP

#include "config_pars.hpp"


void		checkConsecutiveSameBraces(const std::string &raw_data);
size_t		findServerBlockEnd(const std::string &raw_data, size_t start, int &braceCount); //find server block end
size_t		skipWhitespace(const std::string &raw_data, size_t start);	//skip whitespace
void		removeLeadingWhitespaces(std::string &string); //remove leading whitespaces
int			checkHostPort(const std::string &host_port); //check host port
int			stringToInt(const std::string &string); //string to int
void		SemiColonCheck(size_t end, std::string raw_data); //check semicolon
void		onlyIpPort(const std::string &ipPort, int &port); //only ip port
void		checkIpPort(const std::string &ipPort, std::string &ip, int &port);
void		checkport(const std::string &port_str, int &port);
std::string	rootValueCheck(t_location_config &location_config, std::string rootValue, std::string &_server_root);
void		removeFSlashes(std::string &input);
void		removLSlashes(std::string &input);
std::string removeMultipleSlashes(const std::string& input);
std::string removeFndLSlashes(std::string & input);
void		extractMultipleCgi(std::vector<std::string> &cgi_ext, const std::string cgi_str);
void		SlashConvert(std::string real_str, std::string merge_str, std::string &store_str);

#endif
