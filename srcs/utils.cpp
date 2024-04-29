
#include <string>
#include "utils.h"

std::string	genRandomFileName(int len)
{
	const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string randomString;

	for (int i = 0; i < len; ++i) 
		randomString += alphanum[rand() % (sizeof(alphanum) - 1)];

	return (randomString);
}


std::string	createTmpFilePath()
{
	std::string filepath;

	while(1)
	{
		filepath = "www/temp/" + genRandomFileName(10) + ".bin";
		if (access(filepath.c_str(), F_OK) != 0)
			break;
	}
	return (filepath);
}

std::vector<std::string>	splitPath(std::string input, char delim)
{
	std::istringstream			iss(input);
	std::string					item;
	std::vector<std::string>	result;
	
	while (std::getline(iss, item, delim))
		result.push_back("/" + item);

	return (result);
}
