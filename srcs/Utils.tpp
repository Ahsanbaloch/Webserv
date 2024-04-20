
#include "Utils.h"

template<typename T>
std::string toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}
