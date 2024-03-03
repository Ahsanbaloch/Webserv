/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 17:10:22 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/02 17:14:27 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "config.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cerr << "Error: Usage: " << av[0] << " <config_file>" << std::endl;
        return 1;
    }
    try
    {
        config config(av[1]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}