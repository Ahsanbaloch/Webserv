/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars_utils.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamesser <mamesser@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 16:51:42 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/16 13:45:44 by mamesser         ###   ########.fr       */
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
