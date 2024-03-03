/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_parts.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 14:45:55 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/03 15:07:31 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_PARTS_HPP
#define SERVER_PARTS_HPP

#include "config.hpp"

class server_parts
{

	private:
		std::string _server_name;
		std::string _server_port;
		std::string index_page;
		std::string _error_page;
		std::string _body_size;
		std::map<int, std::vector<std::string> > _location;
	public:
		server_parts();
		~server_parts();
		
		std::map<int, std::vector<std::string> > get_location();
		void set_location(std::map<int, std::vector<std::string> > location);
};

#endif