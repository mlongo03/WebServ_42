#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

#include "Server.hpp"
#include "ConfigParser.hpp"
#include "Utils.hpp"

//allow GET POST DELETE only

class Location {
public:
    std::string path;
    std::string index;  //this is present also in Server class
    std::vector<std::string> allow; // this is a list of HTTP methods it allows only GET POST DELETE
    // std::string autoindex;
    // std::string root;

	//getters
	std::vector<std::string> getAllow();

	//setters
	void setIndex(const std::string& index);
	void setPath(const std::string& path);
	void setAllow(const std::vector<std::string>& allow);

    Location();
	~Location();
	Location(const Location& location);
	Location& operator=(const Location& location);

    void print() const;
};



#endif
