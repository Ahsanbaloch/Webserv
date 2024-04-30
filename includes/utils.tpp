#ifndef UTILS_TPP
# define UTILS_TPP

#include <string>
#include <sstream>

template<typename T> std::string toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

template<typename T> std::string toHex(const T& value)
{
	std::stringstream oss;
	oss << std::hex << value;
	return (oss.str());
}

template<typename T> int toInt(const T& value)
{
	std::stringstream oss;
	oss << value;
	int result;
	oss >> result;
	return (result);
}



#endif
