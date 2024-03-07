/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars_utils.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 16:53:25 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/07 11:58:52 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_pars_utils.hpp"

void checkConsecutiveSameBraces(const std::string &raw_data)
{
    size_t pos = 0;
    while (pos < raw_data.size())
    {
       if (raw_data[pos] == '{')
       {
            char brace = raw_data[pos];
            pos++;
            while (pos < raw_data.length() && 
                   (raw_data[pos] == ' ' || raw_data[pos] == '\t' || raw_data[pos] == '\n')) 
                pos++;
            if (pos < raw_data.length() && raw_data[pos] == brace)
                throw std::runtime_error("Consecutive same braces");
       }
        pos++;
    }
}

// Function to skip whitespace
size_t skipWhitespace(const std::string &raw_data, size_t start)
{
    while (start < raw_data.length() && (raw_data[start] == ' ' || raw_data[start] == '\t' || raw_data[start] == '\n'))
        start++;
    return start;
}

// Function to find the end of a server block
size_t findServerBlockEnd(const std::string &raw_data, size_t start, int &braceCount)
{
    size_t end = start + 1;
    while (end < raw_data.length() && braceCount > 0)
    {
        if (raw_data[end] == '{')
            braceCount++;
        else if (raw_data[end] == '}')
            braceCount--;
        end++;
    }
    return end;
}

void	removeLeadingWhitespaces(std::string &string) 
{

	size_t	start = 0;
	while (start < string.size() && (string[start] == ' ' || string[start] == '\t')) {
		start++;
	}
	string = string.substr(start, string.size() - start);
}