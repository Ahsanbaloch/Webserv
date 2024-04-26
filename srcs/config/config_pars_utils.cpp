/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars_utils.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 16:53:25 by ahsalam           #+#    #+#             */
/*   Updated: 2024/04/26 21:18:54 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/config_pars_utils.hpp"

void	checkConsecutiveSameBraces(const std::string &raw_data)
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
				throw std::runtime_error(RED "Consecutive same braces" RESET);
			else if (pos < raw_data.length() && raw_data[pos] == '}')
				throw std::runtime_error(RED "Consecutive braces together without data" RESET);
		}
		pos++;
	}
}

// Function to skip whitespace
size_t skipWhitespace(const std::string &raw_data, size_t start)
{
	while (start < raw_data.length() && (raw_data[start] == ' ' || raw_data[start] == '\t' || raw_data[start] == '\n'))
		start++;
	return (start);
}

// Function to find the end of a server block
size_t	findServerBlockEnd(const std::string &raw_data, size_t start, int &braceCount)
{
	size_t end = start + 1;
	while (end < raw_data.length() && (raw_data.substr(end, 6) != "server"))
	{
		if (raw_data[end] == '{')
			braceCount++;
		else if (raw_data[end] == '}')
			braceCount--;
		end++;
	}
	return (end);
}

void	removeLeadingWhitespaces(std::string &string) 
{
	size_t	start = 0;
	while (start < string.size() && (string[start] == ' ' || string[start] == '\t'))
		start++;
	string = string.substr(start, string.size() - start);
}

void	SemiColonCheck(size_t end, std::string raw_data)
{
	std::string colon = raw_data.substr(end, 1);
	if (colon != ";")
		throw MissingSemicolonException();
}

int	checkHostPort(const std::string &host_port)
{
	std::string host;
	std::string port_str;
	size_t start = 0;
	int port = 0;
	size_t end = host_port.length() - 1;
	if ((start = host_port.find(':', start)) != std::string::npos)
	{
		host = host_port.substr(0, host_port.find(':', start));
		if (host == "127.0.0.1" || host == "localhost" || host == "0.0.0.0")
		{
			port_str = host_port.substr(host_port.find(':', start) + 1, end);
			if (port_str.empty())
				return (9999);
			std::istringstream ss(port_str);
			if (ss >> port)
				return port;
			else
				throw InvalidPortException();
		}
		else
			throw InvalidHostException();
	}
	return (9999);
}


void	onlyIpPort(const std::string &ipPort, int &port)
{
	if (ipPort == "localhost" || ipPort == "" || ipPort == "127.0.0.1" || ipPort == "0.0.0.0")
		return ;
	else
		checkport(ipPort, port);
}

void	checkIpPort(const std::string &ipPort, std::string &ip, int &port)
{
	size_t start = 0;
	size_t end = 0;
	if ((start = ipPort.find(':', start)) != std::string::npos)
	{
		std::string ip1 = ipPort.substr(0, start);
		if (ip1 == "localhost" || ip1 == "127.0.0.1" || ip1 == "0.0.0.0")
			ip = ip1;
		else
			throw InvalidHostException();
		end = ipPort.find(";", start);
		const std::string port_str = ipPort.substr(start + 1, end - start);
		checkport(port_str, port);
	}
}

void	checkport(const std::string &port_str, int &port)
{
	std::istringstream ss(port_str);
	if (ss >> port)
	{
		if (port < 0 || port > 9999)
			throw InvalidPortException();
	}
	else
		throw InvalidPortException();
}

//root insertion and  valueCheck for root
std::string rootValueCheck(t_location_config &location_config, std::string rootValue, std::string &_server_root)
{
	rootValue = removeMultipleSlashes(rootValue);
	_server_root = removeMultipleSlashes(_server_root);
	(void)location_config;
	if (rootValue.empty() && _server_root.empty())
		throw MissingValueException("Root : ");
	else if (!rootValue.empty())
		location_config.root = removeFndLSlashes(rootValue);
	else
		location_config.root = removeFndLSlashes(_server_root);
	return (location_config.root);
}

void removeFSlashes(std::string &input)
{
	if (input[0] == '/')
		input = input.substr(1);
}

void removLSlashes(std::string &input)
{
	if (input[input.length() - 1] == '/')
		input = input.substr(0, (input.length() - 1));
}
std::string removeFndLSlashes(std::string & input)
{
	removeFSlashes(input);
	removLSlashes(input);
	return (input);
}

void SlashConvert(std::string real_str, std::string merge_str, std::string &store_str)
{
	real_str = merge_str + real_str;
	real_str = removeMultipleSlashes(real_str);
	removLSlashes(real_str);
	store_str = real_str;
}

std::string removeMultipleSlashes(const std::string& input) 
{
    std::string result;
    char lastChar = 0;
    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) 
	{
        char c = *it;
        if (c != '/' || lastChar != '/')
            result += c;
        lastChar = c;
    }
    return (result);
}

void extractMultipleCgi(std::vector<std::string> &cgi_ext, const std::string cgi_str)
{
	//std::vector<std::string> temp_cgi;
	std::istringstream ss(cgi_str);
	std::string token;
	while (ss >> token)
	{
		if (token[0] == '.')
		{
			std::string newToken;
			for (std::string::iterator it = token.begin(); it != token.end(); ++it)
			{
				if (*it != ' ')
					newToken += *it;
			}
			//std::cout << "Token : " << newToken << std::endl;
			cgi_ext.push_back(newToken);
		}
	}
}
