/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_pars.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 14:22:42 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/06 16:41:24 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARS_HPP
#define CONFIG_PARS_HPP

#include "config_struct.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include "config_pars_utils.hpp"

class config_pars
{
	private:
	std::map<int, std::map<std::string, t_server_config> > _configMap;
	//config_pars();
	//void parse_server(std::string &server_config);
	

	public: 
	config_pars(char *argv);
	//config_pars(const config_pars &other);
	~config_pars();

	std::map<int, std::map<std::string, t_server_config> >&	getConfigMap(); //getters
	void readconfig(char *argv, std::string &fileContent);	//read config file
	void parse_server_configs(std::string &sever_config); //using this fucntion to parse everything
	//void extractServerBlocks(std::vector<std::string> &serverBlocks, std::string &server_config);
	void extractServer(std::vector<std::string> &server_block,const std::string &server_config);  //separating every server block
	//void findAndCheckServerBlocks(const std::string &raw_data, size_t &start, size_t &end); //find and check server blocks
	void parse_server_block(t_server_config &server_config, const std::string &server_block); //parse inside server block
	std::string extractServerName(int num, const std::string &server_block); //extract server name
	int extractListen(int num, const std::string &server_block); //extract listen
	std::string extractErrorPage(int num, const std::string &server_block); //extract error page
	void Location_block(t_server_config &server_config, const std::string &server_block);
	
	
};

#endif