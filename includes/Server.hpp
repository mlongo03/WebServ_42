#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"
#include "Utils.hpp"

class Location;

class Server
{
private:
	std::vector<std::string> server_names;
	std::string host;
	std::string listen;
	std::string root;
	std::string index;
	std::string _error_page_400;
	std::string _error_page_401;
	std::string _error_page_403;
	std::string _error_page_404;
	std::string _error_page_405;
	std::string _error_page_413;
	std::string _error_page_414;
	std::string _error_page_500;
	std::string _error_page_502;
	std::string _error_page_503;
	size_t _client_max_body_size;
	std::vector<std::string> _cgi_extension;
	bool _autoindex;
	std::vector<std::string> _allow;

	std::vector<Location> locations;

public:
	Server();
	~Server();
	Server(const Server &server);
	Server &operator=(const Server &server);
	bool operator==(const Server &server);
	bool equalLocations(std::vector<Location> locations);

	// getters
	std::vector<std::string> getServerNames() const;
	std::string getHost() const;
	std::string getListen() const;
	std::string getRoot() const;
	std::string getIndex() const;
	std::string getErrorPage400() const;
	std::string getErrorPage401() const;
	std::string getErrorPage403() const;
	std::string getErrorPage404() const;
	std::string getErrorPage405() const;
	std::string getErrorPage413() const;
	std::string getErrorPage414() const;
	std::string getErrorPage500() const;
	std::string getErrorPage502() const;
	std::string getErrorPage503() const;

	size_t getClientMaxBodySize() const;
	std::vector<std::string> getCgiExtension() const;
	std::vector<Location> getLocations() const;
	bool getAutoindex() const;
	std::vector<std::string> getAllow() const;

	// setters
	void setServerNames(const std::vector<std::string> &server_names);
	void setHost(const std::string &host);
	void setListen(const std::string &listen);
	void setRoot(const std::string &root);
	void setIndex(const std::string &index);
	void setErrorPage400(const std::string &error_page_400);
	void setErrorPage401(const std::string &error_page_401);
	void setErrorPage403(const std::string &error_page_403);
	void setErrorPage404(const std::string &error_page_404);
	void setErrorPage405(const std::string &error_page_404);
	void setErrorPage413(const std::string &error_page_413);
	void setErrorPage414(const std::string &error_page_414);
	void setErrorPage500(const std::string &error_page_500);
	void setErrorPage502(const std::string &error_page_502);
	void setErrorPage503(const std::string &error_page_503);
	void setClientMaxBodySize(size_t &client_max_body_size);
	void setCgiExtension(const std::vector<std::string> &cgi_extension);
	void addLocation(const Location &location);
	void setAutoindex(bool autoindex);
	void setAllow(const std::vector<std::string> &allow);
};
std::ostream &operator<<(std::ostream &os, const Server &server);

#endif
