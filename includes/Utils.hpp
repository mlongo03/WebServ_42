#ifndef UTILS_HPP
#define UTILS_HPP

#include "Server.hpp"
#include "Location.hpp"
#include <algorithm>
#include <string>   // for std::string and std::stoi
#include <cctype>   // for ::isdigit
#include <cstdlib>  // for std::atoi

class Server;
class Location;


bool isValidHttpMethod(const std::string& method);
bool isValidNumber(const std::string& str);
std::string trimSpaces(const std::string& str);
std::string trimSemicolons(const std::string& str);
bool hasValidExtension(const std::string& filename);
bool isValidFile(const std::string& filename);
std::string getAbsolutePath(const std::string &path);
bool isFileEmpty(const std::string& filePath);


#endif
