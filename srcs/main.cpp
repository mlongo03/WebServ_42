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


// //// test cgi request
// std::string handleCgiRequest(const std::string &getUrl, const std::string &getMethod) {
// 	std::cout << "----- Handling CGI Request -----" << std::endl;
// 	std::cout << "URL: " << getUrl << std::endl;
// 	std::string cgiBin = "/cgi-bin/";
// 	std::string cgiRoot = "/var/www/cgi-bin/";
// 	std::vector<std::string> cgiExtensions;
// 	cgiExtensions.push_back(".py");
// 	// cgiExtensions.push_back(".pl");

// 	Cgi cgiHandler = Cgi(cgiBin, cgiRoot, cgiExtensions);

// 	std::string postBody = "name=John&age=30";

// 	if (cgiHandler.isCgiRequest(getUrl)) {
// 		cgiHandler.setMethod(getMethod);

// 		// Extract path info (script path) and query string
// 		std::string scriptPath = getUrl.substr(cgiBin.length(), getUrl.find('?') - cgiBin.length());
// 		std::cout << "script path is " << scriptPath << std::endl;
// 		// if (scriptPath.empty()) {
// 		// 	return cgiHandler.generateErrorResponse(404, "Not Found", "No script specified");
// 		// }
//         // if (access(scriptPath.c_str(), F_OK) == -1) {
//         //     // FastCGI (Nginx) returns a 403 error
//         //     return cgiHandler.generateErrorResponse(403, "Forbidden", "Script does not exist");
//         // }
//         // if (access(scriptPath.c_str(), X_OK) == -1) {
//         //     // FastCGI (Nginx) returns a 403 error
//         //     return cgiHandler.generateErrorResponse(403, "Forbidden", "Script has no execute permission");
//         // }
//         cgiHandler.setScriptPath(scriptPath);

//         // Extract query string from the URL
//         std::string queryString;
//         if (getUrl.find('?') == std::string::npos) {
//             queryString = "";
//         } else {
//             queryString = getUrl.substr(getUrl.find('?') + 1);
//         }
//         cgiHandler.prepareEnvVars(queryString);

//         // Execute the CGI script and get the output
//         try {
//             std::cout << "#### Cgi Script response is " << cgiHandler.getMethod() << " :" << std::endl;
//             std::string getResponse = cgiHandler.execute();
//             std::cout << getResponse << std::endl;
//             return getResponse;
//         } catch (const std::exception &e) {
//             std::cerr << "Error executing CGI script: " << e.what() << std::endl;
//             return std::string("Error executing CGI script: ") + e.what();
//         }
//     }
//     return "Error: Not a CGI request";
// }

std::string handleCgiRequest(const std::string &getUrl, const std::string &getMethod, const std::string &queryString, const std::string &postBody, const std::string &contentType)
{
	// Define CGI attributes (normally parsed from the configuration file)
	std::cout << "url is " << getUrl << std::endl;
    std::string cgiBin = "/cgi-bin/";
    std::string cgiRoot = "/var/www/cgi-bin/";
    std::vector<std::string> cgiExtensions;
    cgiExtensions.push_back(".py");
    cgiExtensions.push_back(".pl");

    Cgi cgiHandler = Cgi(cgiBin, cgiRoot, cgiExtensions);

    // std::string postBody = "name=John&age=30";

    if (cgiHandler.isCgiRequest(getUrl)) {
		//set the method of the HTTP
        cgiHandler.setMethod(getMethod);

        // Extract path info (script path) and query string
        std::string scriptPath = getUrl.substr(cgiBin.length(), getUrl.find('?') - cgiBin.length());
        if (scriptPath.empty()) {
            return cgiHandler.generateErrorResponse(404, "Not Found", "No script specified");
        }

        // Prepend cgiRoot to scriptPath to get the full path because acces need the relative path to the file for working
        std::string fullScriptPath = cgiRoot + scriptPath;
		fullScriptPath = cgiHandler.makeRelativePath(fullScriptPath); // Remove leading '/'
        std::cout << "Full script path is " << fullScriptPath << std::endl;

        // Check if the script exists
        if (access(fullScriptPath.c_str(), F_OK) == -1) {
            std::cerr << "Script does not exist: " << fullScriptPath << std::endl;
            return cgiHandler.generateErrorResponse(403, "Forbidden", "Script does not exist");
        }

        // Check if the script has execute permission
        if (access(fullScriptPath.c_str(), X_OK) == -1) {
            std::cerr << "Script has no execute permission: " << fullScriptPath << std::endl;
            return cgiHandler.generateErrorResponse(403, "Forbidden", "Script has no execute permission");
        }

		//now we can set the path to use the fullScrpiptPath
        cgiHandler.setPath(fullScriptPath);

        cgiHandler.prepareEnvVars(queryString, postBody,contentType);

		// Execute the CGI script and get the output
		try {
			std::string getResponse = cgiHandler.execute();
			// std::cout << getResponse << std::endl;
			return getResponse;
		} catch (const std::exception &e) {
			std::cerr << "Error executing CGI script: " << e.what() << std::endl;
			return cgiHandler.generateErrorResponse(500, "Internal Server Error", "Error executing CGI script");
		}
	}
	return cgiHandler.generateErrorResponse(404, "Not Found", "Not a CGI request");
}

int  main() {

	// Simulate a GET request
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

	std::string getUrl = "/cgi-bin/testGET.pl?name=John&age=30";
	std::string getMethod = "GET";
	std::string queryString = "name=John&age=30";
	std::string body = ""; // GET does not send a body
	std::string contentType = ""; // GET does not specifify contentType because it does not send a body


	std::string response = handleCgiRequest(getUrl, getMethod,queryString , body , contentType);
	std::cout << "### Response is: " << std::endl;
	std::cout << response << std::endl;
	std::cout << "###" << std::endl;

	// Simulate a POST request
	// we need an http request parsed from the webserver
	// after this we have to parse all this data
	// for example a POST request should be
	//
	// POST /submit-form HTTP/1.1
	// Host: example.com
	// Content-Type: application/x-www-form-urlencoded
	// Content-Length: 27

	// name=John&age=30


	// std::string postUrl = "/cgi-bin/upload.py";
	// std::string url = "/cgi-bin/testPOST.pl";
	// std::string querystring = "";
	// std::string method = "POST";
	// std::string postBody = "name=John&age=30";
	// std::string contentType = "application/x-www-form-urlencoded";

	// std::string response = handleCgiRequest(url, method,querystring,postBody,contentType);
	// std::cout << "### Response is: " << std::endl;
	// std::cout << response << std::endl;
	// std::cout << "###" << std::endl;


    return 0;
}



// int main() {


//     // // Simulate a GET request
//     // std::string getUrl = "/cgi-bin/test.py?name=John&age=30";
//     // // std::string getUrl = "/cgi-bin/noPerm.py";

//     // std::string getMethod = "GET";
//     // std::string getBody = "";  // GET does not send a body

//     // Simulate a POST request
//     std::string postUrl = "/cgi-bin/upload.py";
//     std::string postMethod = "POST";
//     std::string postBody = "name=John&age=30";  // POST sends data in the body

//     // Define CGI attributes (normally parsed from the configuration file)
//     std::string cgiBin = "/cgi-bin/";
//     std::string cgiRoot = "/var/www/cgi-bin/";  // Relative path
//     std::vector<std::string> cgiExtensions;
//     cgiExtensions.push_back(".py");

//     // // Instantiate the Cgi class
//     Cgi cgiHandler(cgiBin, cgiRoot, cgiExtensions);

// 	// std::cout << "----- Handling GET Request -----" << std::endl;
// 	// if (cgiHandler.isCgiRequest(getUrl)) {
// 	// 	cgiHandler.setMethod(getMethod);

// 	// 	// Extract path info (script path) and query string
// 	// 	std::string scriptPath = getUrl.substr(cgiBin.length(), getUrl.find('?') - cgiBin.length());

// 	// 	std::cout << "script path is " << scriptPath << std::endl;
// 	// 	// if (access((scriptPath).c_str(), F_OK) == -1) {
// 	// 	// 	//fastcgi (ngnix) return a 403 error
// 	// 	// 	std::cout << "Script does not exist" << std::endl;
// 	// 	// 	return 0;
// 	// 	// }
// 	// 	cgiHandler.setScriptPath(scriptPath);

// 	// 	// Extract query string from the URL
// 	// 	if (getUrl.find('?') == std::string::npos) {
// 	// 		std::string queryString = "";
// 	// 	} else
// 	// 	{
// 	// 		std::string queryString = getUrl.substr(getUrl.find('?') + 1);
// 	// 		cgiHandler.prepareEnvVars(queryString);
// 	// 	}

// 	// 	// Execute the CGI script and get the output
// 	// 	try {
// 	// 		std::cout << "#### Cgi Script GET response is:" << std::endl;
// 	// 		std::string getResponse = cgiHandler.execute();
// 	// 		std::cout << getResponse << std::endl;
// 	// 	} catch (const std::exception &e) {
// 	// 		std::cerr << "Error executing CGI script: " << e.what() << std::endl;
// 	// 	}
// 	// } else {
// 	// 	std::cout << "Not a CGI request." << std::endl;
// 	// }

// 	// Handling POST request
// 	std::cout << "\n----- Handling POST Request -----" << std::endl;
// 	if (cgiHandler.isCgiRequest(postUrl)) {
// 		cgiHandler.setMethod(postMethod);
// 		cgiHandler.setScriptPath(postUrl.substr(cgiBin.length()));  // Extract path info
// 		cgiHandler.setBody(postBody);

// 		// Prepare environment variables (no QUERY_STRING for POST)
// 		cgiHandler.prepareEnvVars("");

// 	// Execute the CGI script and get the output
// 	try {
// 		std::cout << "#### Cgi Script Post Response is:" << std::endl;
// 		std::string postResponse = cgiHandler.execute();
// 		std::cout << postResponse << std::endl;
// 		std::cout << "####" << std::endl;
// 	} catch (const std::exception &e) {
// 		std::cerr << "Error executing CGI script: " << e.what() << std::endl;
// 	}
// 	} else {
// 		std::cout << "Not a CGI request." << std::endl;
// 	}

//     return 0;
// }

