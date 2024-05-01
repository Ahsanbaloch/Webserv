#ifndef UTILS_H
# define UTILS_H

# include <fcntl.h>
# include <unistd.h>
# include <cstdlib>
# include <sstream>
# include <string>
# include <vector>

std::string	createTmpFilePath();
std::string	genRandomFileName(int);
std::vector<std::string>	splitPath(std::string input, char delim);

#endif
