#include "Location.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
#include "Utils.hpp"
#include "Worker.hpp"
#include "Request.hpp"
#include "Cgi.hpp"


// int main(int argc, char **argv)
// {
// 	if (argc != 2)
// 	{
// 		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}
// 	else
// 	{
// 		try
// 		{
// 			ConfigParser configParser;
// 			Worker worker = Worker(configParser.parseConfig(argv[1]));
// 			worker.run();
// 		}
// 		catch (std::exception &e)
// 		{
// 			std::cerr << e.what() << std::endl;
// 			exit(EXIT_FAILURE);
// 		}
// 	}
// 	return 0;
// }



int  main() {

// std::string rawRequest =
//         "GET ./var/www/cgi-bin/index.html HTTP/1.1\r\n"
//         "Host: www.example.com\r\n"
//         "User-Agent: Mozilla/5.0\r\n"
// 		"Content-Type: text/html\r\n"
//         "Accept: text/html\r\n"
//         "\r\n";

//     // Create a Request object
//     Request request(rawRequest);


	std::string realpath = "./var/www/cgi-bin/testGET.pl";
	// std::map<std::string, std::string> queryString;
	// queryString["name"] = "John";
	// queryString["age"] = "30";
	// queryString["HTTP/1.1"] = "HTTP/1.1";
	// queryString["lol"] = "dsf";
    std::vector<std::string> cgiExtensions;
    cgiExtensions.push_back(".py");
    cgiExtensions.push_back(".pl");

    Cgi cgiHandler = Cgi(realpath,cgiExtensions);
	//next is to handle the request
	// Cgi cgiHandler2 = Cgi(request,cgiExtensions);
	
	//// request parsed from the webserver he can have a query string or not

	// std::string path = "/cgi-binfd/testGET.pl";
	//// std::string path = "/cgi-bin/testGET.pl";
	std::string getMethod = "GET";
	std::string body = ""; // GET does not send a body
	std::string contentType = ""; // GET does not specifify contentType because it does not send a body


	std::string response = cgiHandler.handleCgiRequest(cgiHandler, getMethod ,body, contentType);
	std::cout << "### Response is: " << std::endl;
	std::cout << response << std::endl;
	std::cout << "###" << std::endl;


    return 0;
}



