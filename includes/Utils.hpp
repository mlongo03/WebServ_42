// utils.hpp
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
bool is_valid_port(const std::string& str);
std::string trim(const std::string& str);

#endif  // UTILS_HPP
