/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverIntegration.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 16:51:42 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/04 19:36:18 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERINTEGRATION_HPP
#define SERVERINTEGRATION_HPP


#include "config_pars.hpp"

void checkBasicServerBlockIntegrity(const std::string &rawConfig, const size_t &start, const size_t &end);

#endif