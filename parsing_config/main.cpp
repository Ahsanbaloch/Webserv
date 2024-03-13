/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 17:10:22 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/12 17:18:34 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "config_pars.hpp"

int main(int argc, char **argv)
{
   /*  if (ac == 1)
    {
        std::cerr << "Error: Usage: " << av[0] << " <config_file>" << std::endl;
        return 1;
    }
    else if (ac != 2)
    {
        std::cerr << "Error: Usage: " << av[0] << " <config_file>" << std::endl;
        return 1;
    } */
    try
    {
        config_pars config(argc, argv);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

