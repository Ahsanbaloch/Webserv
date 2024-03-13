/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 14:22:42 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/13 19:33:26 by ahsalam          ###   ########.fr       */
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


/* In C++98, you can't use a std::vector<int> as a key in a std::map directly 
because there's no operator< defined for std::vector<int>. 
However, you can define a custom comparison function and use that to create a std::map
 with std::vector<int> as the key. */

/* 
 */

class config_pars
{
	private:
	std::map<int, std::map<std::string, t_server_config> > _configMap;
	//config_pars();
	//void parse_server(std::string &server_config);
	std::vector<t_server_config> _server_configs_vector;

	public: 
	config_pars(int argc, char **argv);
	//config_pars(const config_pars &other);
	~config_pars();
	//std::map<int, std::map<std::string, t_server_config> >&	getConfigMap();

	std::vector<t_server_config> &getServerConfigsVector();
	std::map<int, std::map<std::string, t_server_config> >&	getConfigMap(); //getters
	void argumentCheck(int argc, char **argv, std::string &argument_value);
	void readconfig(std::string &argv, std::string &fileContent);	//read config file
	void parse_server_configs(std::string &sever_config); //using this fucntion to parse everything
	//void extractServerBlocks(std::vector<std::string> &serverBlocks, std::string &server_config);
	void extractServer(std::vector<std::string> &server_block,const std::string &server_config);  //separating every server block
	//void findAndCheckServerBlocks(const std::string &raw_data, size_t &start, size_t &end); //find and check server blocks
	void parse_server_block(t_server_config &server_config, const std::string &server_block); //parse inside server block
	std::string extractServerName(const std::string &server_block); //extract server name
	int extractListen(const std::string &server_block); //extract listen
	//std::vector<int> extractListen(const std::string &server_block); //extract listen
	std::string extractErrorPage(const std::string &server_block); //extract error page
	/* Location block.... */
	void Location_block(t_server_config &server_config, const std::string &server_block); //parse location block
	void splitLocationBlocks(std::vector<std::string> &location_blocks, const std::string &server_block); //split location blocks
	void parseLocationBlock(t_location_config &location_config, const std::string &location_block); //parse location block
	std::string extractPath(const std::string &location_block); //extract path
	std::string extractVariables(const std::string &variable, const std::string &location_block); //extract variables
	bool extractAutoIndex(const std::string &location_block); //extract autoindex
	int	extractBodySize(const std::string &server_block); //extract body size
};

#endif