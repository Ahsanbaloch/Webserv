/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars_utils.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 16:51:42 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/17 17:26:24 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARS_UTILS_HPP
#define CONFIG_PARS_UTILS_HPP

#include "config_pars.hpp"


void checkConsecutiveSameBraces(const std::string &raw_data);
size_t findServerBlockEnd(const std::string &raw_data, size_t start, int &braceCount); //find server block end
size_t skipWhitespace(const std::string &raw_data, size_t start);	//skip whitespace
void    removeLeadingWhitespaces(std::string &string); //remove leading whitespaces
int checkHostPort(const std::string &host_port); //check host port
int stringToInt(const std::string &string); //string to int
void SemiColonCheck(size_t end, std::string raw_data); //check semicolon

void onlyIpPort(const std::string &ipPort, int &port); //only ip port
void checkIpPort(const std::string &ipPort, std::string &ip, int &port);
void checkport(const std::string &port_str, int &port);
#endif
