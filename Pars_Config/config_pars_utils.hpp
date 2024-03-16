/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars_utils.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 16:51:42 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/13 13:26:07 by ahsalam          ###   ########.fr       */
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
#endif