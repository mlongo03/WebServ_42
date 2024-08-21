#include "Location.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
#include "Utils.hpp"
#include "Worker.hpp"
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

// test fake GET request
// int main() {
//     // Simulate a fake HTTP request
//     std::string url = "/cgi-bin/testGET.py";
//     std::string method = "GET"; // or "POST"
//     std::string body = "";      // Empty for GET, could contain data for POST

//     // Define CGI attributes (normally parsed from the configuration file)
//     std::string cgiBin = "/cgi-bin/";
//     std::string cgiRoot = "/var/www/cgi-bin/";
//     std::vector<std::string> cgiExtensions;
//     cgiExtensions.push_back(".py");

//     // Instantiate the Cgi class
//     Cgi cgiHandler(cgiBin, cgiRoot, cgiExtensions);

//     // Check if the request is a CGI request
//     if (cgiHandler.isCgiRequest(url)) {
// 		std::cout <<"url is " << url << std::endl;
//         // Set the method and script path
//         cgiHandler.setMethod(method);
//         cgiHandler.setScriptPath(url.substr(cgiBin.length())); // Extract path info

//         // Prepare environment variables (e.g., PATH_INFO)
//         cgiHandler.prepareEnvVars(url.substr(cgiBin.length()));

//         // If it's a POST request, set the body
//         if (method == "POST") {
//             cgiHandler.setBody(body);
//         }

//         // Execute the CGI script and get the output
//         try {
//             std::cout << "Inside the try block" << std::endl;
//             std::string response = cgiHandler.execute();
//             std::cout << "CGI Script Output:\n" << response << std::endl;
//         } catch (const std::exception &e) {
//             std::cerr << "Error executing CGI script: " << e.what() << std::endl;
//         }
//     } else {
//         std::cout << "Not a CGI request." << std::endl;
//     }

//     return 0;
// }


// int main() {
//     // Simulate a fake HTTP POST request
//     std::string url = "/cgi-bin/testPOST.py";
//     std::string method = "POST";
//     std::string body = "name=John&age=30";  // Example POST data

//     // Define CGI attributes (normally parsed from the configuration file)
//     std::string cgiBin = "/cgi-bin/";
//     std::string cgiRoot = "/var/www/cgi-bin/";
//     std::vector<std::string> cgiExtensions;
//     cgiExtensions.push_back(".py");

//     // Instantiate the Cgi class
//     Cgi cgiHandler(cgiBin, cgiRoot, cgiExtensions);

//     // Check if the request is a CGI request
//     if (cgiHandler.isCgiRequest(url)) {
//         // Set the method and script path
//         cgiHandler.setMethod(method);
//         cgiHandler.setScriptPath(url.substr(cgiBin.length())); // Extract path info

//         // Prepare environment variables (e.g., PATH_INFO)
//         cgiHandler.prepareEnvVars(url.substr(cgiBin.length()));

//         // Set the body for POST requests
//         cgiHandler.setBody(body);

//         // Execute the CGI script and get the output
//         try {
//             std::string response = cgiHandler.execute();
//             std::cout << "CGI Script Output:\n" << response << std::endl;
//         } catch (const std::exception &e) {
//             std::cerr << "Error executing CGI script: " << e.what() << std::endl;
//         }
//     } else {
//         std::cout << "Not a CGI request." << std::endl;
//     }
// }

int main() {
    // Simulate a GET request
    std::string getUrl = "/cgi-bin/test.py?name=John&age=30";
    std::string getMethod = "GET";
    std::string getBody = "";  // GET does not send a body

    // Simulate a POST request
    std::string postUrl = "/cgi-bin/test.py";
    std::string postMethod = "POST";
    std::string postBody = "name=Johnny&age=30";  // POST sends data in the body

    // Define CGI attributes (normally parsed from the configuration file)
    std::string cgiBin = "/cgi-bin/";
    std::string cgiRoot = "/var/www/cgi-bin/";  // Relative path
    std::vector<std::string> cgiExtensions;
    cgiExtensions.push_back(".py");

    // Instantiate the Cgi class
    Cgi cgiHandler(cgiBin, cgiRoot, cgiExtensions);

// std::cout << "----- Handling GET Request -----" << std::endl;
// if (cgiHandler.isCgiRequest(getUrl)) {
//     cgiHandler.setMethod(getMethod);

//     // Extract path info (script path) and query string
//     std::string scriptPath = getUrl.substr(cgiBin.length(), getUrl.find('?') - cgiBin.length());
//     cgiHandler.setScriptPath(scriptPath);

//     // Extract query string from the URL
//     std::string queryString = getUrl.substr(getUrl.find('?') + 1);
//     cgiHandler.prepareEnvVars(queryString);

//     // Execute the CGI script and get the output
//     try {
//         std::string getResponse = cgiHandler.execute();
//         std::cout << "CGI Script response - Output (GET):\n" << getResponse << std::endl;
//     } catch (const std::exception &e) {
//         std::cerr << "Error executing CGI script: " << e.what() << std::endl;
//     }
// } else {
//     std::cout << "Not a CGI request." << std::endl;
// }

    // Handling POST request
    std::cout << "\n----- Handling POST Request -----" << std::endl;
    if (cgiHandler.isCgiRequest(postUrl)) {
        cgiHandler.setMethod(postMethod);
        cgiHandler.setScriptPath(postUrl.substr(cgiBin.length()));  // Extract path info
        cgiHandler.setBody(postBody);

        // Prepare environment variables (no QUERY_STRING for POST)
        cgiHandler.prepareEnvVars("");

        // Execute the CGI script and get the output
        try {
            std::string postResponse = cgiHandler.execute();
            std::cout << "CGI Script Response in Output (POST):\n" << postResponse << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error executing CGI script: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Not a CGI request." << std::endl;
    }

    return 0;
}

