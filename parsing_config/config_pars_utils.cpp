/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars_utils.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 16:53:25 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/05 18:23:23 by ahsalam          ###   ########.fr       */
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

/* 
void config_pars::findServerBlockEnd(const std::string &raw_data, size_t &start, size_t &end)
{
	int braceCount = 1;
	end = start + 1;
	while (end < raw_data.length())
	{
		if (raw_data[end] == '{')
			braceCount++;
		else if (raw_data[end] == '}')
			braceCount--;
		end++;
		//if (braceCount == 0) //check this condition later
		  //  break; 
	}
	if (braceCount > 0)
		throw std::runtime_error("Malformed server block");
	else if (braceCount < 0)
		throw std::runtime_error("Malformed server block");
}

void config_pars::findAndCheckServerBlocks(const std::string &raw_data, size_t &start, size_t &end)
{
	size_t serverNum = 0;
	while ((start = raw_data.find("server", start)) != std::string::npos)
	{
		start += 6;
		while (start < raw_data.length() && (raw_data[start] == ' ' || raw_data[start] == '\t' || raw_data[start] == '\n'))
			start++;
		if (raw_data[start] != '{')
		{
			start++;
			continue;
		}
		findServerBlockEnd(raw_data, start, end);
		serverNum++;
		if (serverNum >= 10)
			throw std::runtime_error("Max capacity"); //TODO: implement this exception
		
	}

}

void config_pars::extractServer(std::vector<std::string> &serevrblock, const std::string &raw_data)
{
	size_t start = 0;
	size_t end = 0;
	checkConsecutiveSameBraces(raw_data); //TODO: implement this function
	findAndCheckServerBlocks(raw_data, start, end); //TODO: implement this function
	std::string serevrBlock = raw_data.substr(start, end - start);
	std::cout << "server block: "  << serevrBlock << std::endl;
	if (serevrBlock.empty())
		std::cout<< "empty server block..." << std::endl; //throw ServerBlockSeparatorException();
	serevrblock.push_back(serevrBlock);
	if (serevrblock.empty())
		std::cout<< "No server block..." << std::endl; //throw NoServerBlockException();
}

 */