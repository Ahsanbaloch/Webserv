/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverIntegration.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 16:53:25 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/04 17:20:50 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverIntegration.hpp"

void checkBasicServerBlockIntegrity(const std::string &rawConfig)
{
	std::string serverBlock;
	std::string::size_type start = 0;
	std::string::size_type end = 0;
	std::string::size_type pos = 0;

	while ((pos = rawConfig.find("server", pos)) != std::string::npos)
	{
		start = rawConfig.find("{", pos);
		end = rawConfig.find("}", start);
		serverBlock = rawConfig.substr(start, end - start);
		// check server block integrity
		pos = end;
	}
} // checkBasicServerBlockIntegrity


/* 
void checkServerBlockSeparators(const std::string &rawConfig,
                                const size_t &start, const size_t &end) {
    if ((rawConfig.find("server {", start) == std::string::npos) ||
        rawConfig.find("server {", start) > end)
        throw ServerBlockSeparatorException();
    size_t serverBlockStartDelimiter = rawConfig.find("server {", start);
    if (rawConfig.find("server {", serverBlockStartDelimiter + 8)
        < rawConfig.find("}", serverBlockStartDelimiter + 8))
        throw ServerBlockSeparatorException();
    size_t serverBlockEndDelimiter = rawConfig.find("}", start);
    if (rawConfig.find("}", serverBlockEndDelimiter + 1)
        < rawConfig.find("server {", serverBlockEndDelimiter + 1))
        throw ServerBlockSeparatorException();
}

 */