/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception_parsing.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 19:37:33 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/08 19:43:59 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exception_parsing.hpp"

const char *InvalidArgNumException::what() const throw()
{
	return (RED "Invalid number of arguments" RESET);
}

const char *CantOpenConfigException::what() const throw()
{
	return (RED "Can't open config file" RESET);
}

const char *EmptyConfigFileException::what() const throw()
{
	return (RED "Empty config file" RESET);
}

const char *DuplicateServerNameException::what() const throw()
{
	return (RED "Duplicate server name" RESET);
}

const char *ValueMissingException::what() const throw()
{
	return (RED "Value missing" RESET);
}

const char *InvalidbodySizeException::what() const throw()
{
	return (RED "Invalid body size" RESET);
}


const char *MissingSemicolonException::what() const throw()
{
	return (RED "Missing semicolon" RESET);
}


const char *MissingClosingBracketException::what() const throw()
{
	return (RED "Missing closing bracket" RESET);
}


const char *InvalidLocationException::what() const throw()
{	
	return (RED "Invalid location" RESET);
}

const char *InvalidPathException::what() const throw()
{
	return (RED "Invalid path" RESET);
}

const char *InvalidHostException::what() const throw()
{
	return (RED "Invalid host" RESET);
}

const char *InvalidPortException::what() const throw()
{
	return (RED "Invalid port" RESET);
}

const char *NoValidServerConfigException::what() const throw()
{
	return (RED "No valid server config" RESET);
}

const char *ExceededMaxServerNumberException::what() const throw()
{
	return (RED "Exceeded max server number" RESET);
}

const char *DuplicateLocationNameException::what() const throw()
{
	return (RED "Duplicate location name" RESET);
}

const char *NoRootLocationException::what() const throw()
{
	return (RED "No root location" RESET);
}

const char *EmptyServerBlockException::what() const throw()
{
	return (RED "Empty server block" RESET);
}