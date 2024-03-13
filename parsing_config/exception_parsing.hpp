/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception_parsing.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 18:11:04 by ahsalam           #+#    #+#             */
/*   Updated: 2024/03/12 17:58:52 by ahsalam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_PARSING_HPP
#define EXCEPTION_PARSING_HPP

#define RED "\033[1;31m"
#define RESET "\033[0m"

#include "config_pars.hpp"

class MissingValueException : public std::exception {
	public:
		virtual const char *what() const throw();
		MissingValueException(const std::string &missingValue);
		virtual ~MissingValueException() throw() {}
	private:
		std::string	_Value;
};

class InvalidArgNumException : public std::exception {
public:
	const char *what() const throw();
};

class CantOpenConfigException : public std::exception { //checked
public:
	const char *what() const throw();
};

class EmptyConfigFileException: public std::exception {
public:
	const char *what() const throw();
};

class DuplicateServerNameException: public std::exception {
public:
	const char *what() const throw();
};

class ValueMissingException : public std::exception { //checked
public:
	const char *what() const throw();
};

class InvalidbodySizeException : public std::exception {
public:
	const char *what() const throw();
};

class MissingSemicolonException : public std::exception {
public:
	const char* what() const throw();
};

class MissingBracketException : public std::exception {
public:
	const char* what() const throw();
};

class InvalidLocationException : public std::exception {
public:
	const char* what() const throw();
};

class InvalidPathException : public std::exception {
public:
	const char* what() const throw();
};

class InvalidHostException : public std::exception { //checked
public:
	const char* what() const throw();
};

class InvalidPortException : public std::exception { //checked
public:
	const char* what() const throw();
};

class NoValidServerConfigException : public std::exception {
public:
	const char* what() const throw();
};

class ExceededMaxServerNumberException : public std::exception {
public:
	const char* what() const throw();
};

class ServerBlockSeparatorException : public std::exception {
public:
	const char* what() const throw();
};

class EmptyServerBlockException : public std::exception {
public:
	const char* what() const throw();
};

class ExceededMaxLocationNumberException : public std::exception {
public:
	const char* what() const throw();
};

class DuplicateLocationNameException : public std::exception {
public:
	const char* what() const throw();
};

class NoRootLocationException : public std::exception {
public:
	const char* what() const throw();
};

class InvalidConfigFileException : public std::exception {
public:
	const char* what() const throw();
};

#endif
