/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_pars_loc.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 13:25:26 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/02 14:15:26 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_pars_loc.hpp"

server_pars_loc::server_pars_loc(/* args */) : config_file()
{
}

server_pars_loc::~server_pars_loc()
{
}

void server_pars_loc::set_server_location_data(std::vector<std::string> location_parsing)
{
	this->location_parsing = location_parsing;
}

std::vector<std::string> server_pars_loc::get_server_location_data() const
{
	return location_parsing;
}
void server_pars_loc::pars_server()
{
	size_t start = 0;
	size_t end = 0;

	for (size_t i = 0; i < file_data.size(); i++)
	{
		while ((start = file_data[i].find("location", end)) != std::string::npos)
		{
			int bracketcount = 0;
			for (end = start; end < file_data[i].size(); ++end)
			{
				if (file_data[i][end] == '{')
					++bracketcount;
				else if (file_data[i][end] == '}')
					--bracketcount;
				if (bracketcount == 0 && file_data[i][end] == '}')
					break;
			}
			location_parsing.push_back(file_data[i].substr(start, end - start + 1));
			end++;
		}
	}
}
