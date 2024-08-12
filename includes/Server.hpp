#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"
#include "Utils.hpp"

class Location;

class Server {
	private:
		std::vector<std::string> server_names;
		std::string host; // not present in ngnix
		std::string listen; // port number as a string
		std::string root; // root directory
		std::string index;  // this can be overridden by location block, if is not present in location block, it will be  the default index of all the locations
		std::string _error_page_400;
		std::string _error_page_401;
		std::string _error_page_403;
		std::string _error_page_404;
		std::string _error_page_500;
		std::string _error_page_502;
		std::string _error_page_503;
		std::vector<Location> locations; // vector of locations

	public:
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
		std::string getErrorPage400() const;
		std::string getErrorPage401() const;
		std::string getErrorPage403() const;
		std::string getErrorPage404() const;
		std::string getErrorPage500() const;
		std::string getErrorPage502() const;
		std::string getErrorPage503() const;


		//setters
		void setServerNames(const std::vector<std::string>& server_names);
		void setHost(const std::string& host);
		void setListen(const std::string& listen);
		void setRoot(const std::string& root);
		void setIndex(const std::string& index);
		void setErrorPage400(const std::string& error_page_400);
		void setErrorPage401(const std::string& error_page_401);
		void setErrorPage403(const std::string& error_page_403);
		void setErrorPage404(const std::string& error_page_404);
		void setErrorPage500(const std::string& error_page_500);
		void setErrorPage502(const std::string& error_page_502);
		void setErrorPage503(const std::string& error_page_503);

		void addLocation(const Location& location);
};
	std::ostream& operator<<(std::ostream& os, const Server& server);

#endif
