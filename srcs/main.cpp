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


// main for cgi
// int  main() {

    // Example raw HTTP request


    // std::string rawRequest = "GET /cgi-bin/testGET.pl HTTP/1.1\r\n"
    //                          "Host: localhost\r\n"
    //                          "User-Agent: Mozilla/5.0\r\n"
    //                          "Accept: text/html\r\n\r\n";

    // // Create a Request object
    // Request request(rawRequest);

    // // Define CGI root and CGI bin
    // std::string cgiRoot = "/var/www/cgi-bin";
    // std::string cgiBin = "/cgi-bin";
	// std::vector<std::string> cgiExtensions;
	// cgiExtensions.push_back(".py");
	// cgiExtensions.push_back(".pl");
    // // Create a Cgi object
    // Cgi cgi(cgiRoot, cgiBin, cgiExtensions );

	// std::cout << "request path is " << request.getPath() << std::endl;
	// std::cout << request.getMethod() << std::endl;

    // //// Check if the request is a CGI request
    // if (cgi.isCgiRequest(request.getPath())) {
	// 	std::cout << "full script path is " << request.getPath() << std::endl;
    //     std::cout << "This is a valid CGI request." << std::endl;
    // } else {
    //     std::cout << "This is not a valid CGI request." << std::endl;
    // }

    ////raw request end main

	// // Simulate a GET request with handleCgiRequest

	// we need an http request parsed from the webserver
	// after this we have to parse all this data
	// for example a GET request should be
	// Method: GET
	// Path: /cgi-bin/testGET.pl?name=John&age=30
	// Query String: name=John&age=30
	// HTTP Version: HTTP/1.1
	// Host: example.com
	// User-Agent: Mozilla/5.0
	// Accept: text/html


	//// Define CGI attributes (normally parsed from the configuration file)
// 	std::string root = "/var/www";
//     std::string cgiBin = "/cgi-bin/";
//     std::vector<std::string> cgiExtensions;
//     cgiExtensions.push_back(".py");
//     cgiExtensions.push_back(".pl");

//     Cgi cgiHandler = Cgi(cgiBin, cgiExtensions);

// 	//// request parsed from the webserver he can have a query string or not

// 	std::string path = "/cgi-bin/testGET.py?name=John&age=30";
// 	//// std::string path = "/cgi-bin/testGET.pl";
// 	std::string getMethod = "GET";
// 	std::string body = ""; // GET does not send a body
// 	std::string contentType = ""; // GET does not specifify contentType because it does not send a body


// 	std::string response = cgiHandler.handleCgiRequest(cgiHandler, path, getMethod ,body, contentType, root);
// 	std::cout << "### Response is: " << std::endl;
// 	std::cout << response << std::endl;
// 	std::cout << "###" << std::endl;


//     return 0;
// }



// main for  various cgi
int  main() {

    // Example raw HTTP request

    // std::string rawRequest = "GET /cgi-bin/testGET.pl HTTP/1.1\r\n"
    //                          "Host: localhost\r\n"
    //                          "User-Agent: Mozilla/5.0\r\n"
    //                          "Accept: text/html\r\n\r\n";

    // // Create a Request object
    // Request request(rawRequest);

    // // Define CGI root and CGI bin
    // std::string cgiRoot = "/var/www/cgi-bin";
    // std::string cgiBin = "/cgi-bin";
	// std::vector<std::string> cgiExtensions;
	// cgiExtensions.push_back(".py");
	// cgiExtensions.push_back(".pl");
    // // Create a Cgi object
    // Cgi cgi(cgiRoot, cgiBin, cgiExtensions );

	// std::cout << "request path is " << request.getPath() << std::endl;
	// std::cout << request.getMethod() << std::endl;

    // //// Check if the request is a CGI request
    // if (cgi.isCgiRequest(request.getPath())) {
	// 	std::cout << "full script path is " << request.getPath() << std::endl;
    //     std::cout << "This is a valid CGI request." << std::endl;
    // } else {
    //     std::cout << "This is not a valid CGI request." << std::endl;
    // }


    //// raw request end main

	//// test for a post request
	////Define CGI attributes (normally parsed from the configuration file)

	std::string root = "/var/www";
	std::string cgiBin = "/cgi-bin/";
    std::vector<std::string> cgiExtensions;
    cgiExtensions.push_back(".py");
    cgiExtensions.push_back(".pl");

    Cgi cgiHandler = Cgi(cgiBin, cgiExtensions);


	std::string path = "/cgi-bin/testPOST.py";
	// std::string path = "/cgi-bin/empty.py"; // empty file with permission
	// std::string path = "/cgi-bin/noPerm.py"; // script without permission

	std::string method = "POST";
	std::string body = "name=John&age=30";
	std::string contentType = "application/x-www-form-urlencoded";

	std::string response = cgiHandler.handleCgiRequest(cgiHandler, path, method,body,contentType,root);
	std::cout << "### Response is: " << std::endl;
	std::cout << response << std::endl;
	std::cout << "###" << std::endl;


    return 0;
}


