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
void printAllServers(std::vector<Server>& servers);
bool isValidPort(const std::string& str);
std::string trimSpaces(const std::string& str);
std::string trimSemicolons(const std::string& str);

#endif
