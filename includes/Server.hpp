#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"
#include "Utils.hpp"

class Location;

class Server {
	public:
		// std::string server_name; original server_name is a string, but we are using a vector of strings
		std::vector<std::string> server_names;
		std::string host; // not present in ngnix
		std::string listen; // port number as a string
		std::string root; // root directory
		std::string index;  // this can be overridden by location block, if is not present in location block, it will be  the default index of all the locations
		std::vector<Location> locations; // vector of locations

		Server();
		~Server();
		Server(const Server& server);
		Server& operator=(const Server& server);

		//getters
		std::vector<std::string> getServerNames() const;
		std::string getHost() const;
		std::string getListen() const;
		std::string getRoot() const;
		std::string getIndex() const;
		std::vector<Location> getLocations()const ;

		//setters
		void setServerNames(const std::vector<std::string>& server_names);
		void setHost(const std::string& host);
		void setListen(const std::string& listen);
		void setRoot(const std::string& root);
		void setIndex(const std::string& index);
		void print() const;
		void printAllServers(std::vector<Server>& servers);

};
	std::ostream& operator<<(std::ostream& os, const Server& server);

#endif
