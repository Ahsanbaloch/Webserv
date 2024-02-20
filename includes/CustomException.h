
#ifndef CUSTOMEXCEPTION_H
# define CUSTOMEXCEPTION_H

# include <string>
# include <stdexcept>

class CustomException: public std::runtime_error
{
	public:
		explicit CustomException(std::string msg): std::runtime_error(msg) {}
};

#endif
