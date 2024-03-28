/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 14:22:42 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/28 22:05:26 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARS_HPP
#define CONFIG_PARS_HPP

#include "config_struct.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include "config_pars_utils.hpp"
#include "exception_parsing.hpp"
#include <set>

class config_pars
{
	private:
		std::map<std::string, std::vector<t_server_config> > _server_configs_map; //getter is getServerConfigsMap()

	
		void		checkDuplicatePath(std::map<std::string, std::vector<t_server_config> > _server_configs_map);
		void		extractIpPort(const std::string &server_block, std::string &ip, int &port); //extract ip and port
		void		checkForDuplicatePaths(std::vector<t_server_config> _server, std::vector<std::string> location_blocks); //check for duplicate paths
		void		argumentCheck(int argc, char **argv, std::string &argument_value);
		void		readconfig(std::string &argv, std::string &fileContent);	//read config file
		void		parse_server_configs(std::string &sever_config); //using this fucntion to parse everything
		void		extractServer(std::vector<std::string> &server_block,const std::string &server_config);  //separating every server block
		void		parse_server_block(t_server_config &server_config, const std::string &server_block); //parse inside server block
		void		Location_block(t_server_config &server_config, const std::string &server_block,  std::string server_root, std::string server_index, std::string error_string); //parse location block
		void		splitLocationBlocks(std::vector<std::string> &location_blocks, const std::string &server_block); //split location blocks
		void		parseLocationBlock(t_location_config &location_config,const std::string &location_block, std::string server_root, std::string server_index, std::string error_string); //parse location block
		std::string	extractPath(const std::string &location_block); //extract path
		std::string extractVariables(const std::string &variable, const std::string &location_block); //extract variables
		bool		extractAutoIndex(const std::string &location_block); //extract autoindex
		void		allowMethods(bool &GET, bool &POST, bool &DELETE, const std::string location_block); //allow methods
		int			extractBodySize(const std::string &server_block); //extract body size
		void 		extractErrorPage(int &status, std::string & page, std::string split_string);
		std::string	extractServerVariable(const std::string variable, const std::string &server_block); //extract server variable
	public:
		config_pars(int argc, char **argv);
		~config_pars();
		std::map<std::string, std::vector<t_server_config> > &getServerConfigsMap();
};

#endif
