
#ifndef CUSTOMEXCEPTION_H
# define CUSTOMEXCEPTION_H

# include <string>

class CustomException : public std::runtime_error
{
	public:
		explicit CustomException(std::string msg): std::runtime_error(msg) {}
};

#endif
