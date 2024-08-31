#include "Cgi.hpp"
#include <limits.h>

	Cgi::Cgi() {
	}

	Cgi::Cgi(const Cgi &src) {
		*this = src;
	}

	Cgi &Cgi::operator=(const Cgi &src) {
		if (this != &src) {
			path_info = src.path_info;
			method = src.method;
			envVars = src.envVars;
			body = src.body;
			script_name = src.script_name;
			cgiExtensions = src.cgiExtensions;
			queryParameters = src.queryParameters;
		}
		return *this;
	}

	Cgi::~Cgi() {}


	Cgi::Cgi(const std::string &path , const std::vector<std::string> &extensions, const Request &request) : path_info(path), cgiExtensions(extensions)
	{
		method = request.getMethod();
		body = request.getBody();
		queryParameters = request.getQueryParameters();
		extract_script_name(path);
	}

	//getters
	std::string Cgi::getScriptName() const {
		return script_name;
	}

	std::string Cgi::getMethod() const {
		return method;
	}

	std::string Cgi::getBody() const {
		return body;
	}

	std::map<std::string, std::string> Cgi::getEnvVars() const {
		return envVars;
	}


	std::vector<std::string> Cgi::getCgiExtensions() const {
		return cgiExtensions;
	}

	std::string Cgi::getPath_info() const {
		return path_info;
	}

	std::map<std::string, std::string> Cgi::getQueryParameters() const {
		return queryParameters;
	}

    // Setters for method, script path, and request body
    void Cgi::setMethod(const std::string &httpMethod) {
        method = httpMethod;
    }

    void Cgi::setScriptName(const std::string &name) {
        script_name = name;
    }

    void Cgi::setBody(const std::string &requestBody) {
        body = requestBody;
    }

	void Cgi::setPath_info (const std::string &path)
	{
		path_info = path;
	}

	void Cgi::setQueryParameters(const std::map<std::string, std::string> &params) {
		queryParameters = params;
	}


bool Cgi::isCgiRequest(const std::string &url) {
    // Check if either '.' or '?' is present in the URL
    if (url.find('.') == std::string::npos) {
		std::cout << "url does not contain '.'" << std::endl;
        return false;
    }
    std::string scriptPath = url.substr(0, url.find('?')); // Remove query string if present
    std::string scriptExtension = scriptPath.substr(scriptPath.find_last_of("."));
    for (std::vector<std::string>::const_iterator it = cgiExtensions.begin(); it != cgiExtensions.end(); ++it) {
        if (scriptExtension == *it) {
            return true;
        }
    }
    return false;
}


void Cgi::prepareEnvVars( const Request &request)
{
    envVars["METHOD"] = request.getMethod();
    envVars["SCRIPT_NAME"] = script_name;
    envVars["PATH_INFO"] = request.getPath();

	if (method == "GET")
	{
		std::map<std::string, std::string> queryParameters = request.getQueryParameters();
		for (std::map<std::string, std::string>::iterator it = queryParameters.begin(); it != queryParameters.end(); ++it)
			envVars[it->first] = it->second;
	}
    else if (method == "POST")
	{
        std::ostringstream oss;
        oss << request.getBody().size();
        envVars["CONTENT_LENGTH"] = oss.str();
        envVars["CONTENT_TYPE"] = request.getContentType();
		std::cout << "content type is " << request.getContentType() << std::endl;
        body = request.getBody();  // Store the POST body for later use
		std::cout << "body is " << body << std::endl;
    }
}

    // Convert map of environment variables to char* array for execve
    char **Cgi::buildEnvArray() {
        char **envArray = new char*[envVars.size() + 1];
        int i = 0;
		for (std::map<std::string, std::string>::const_iterator it = envVars.begin(); it != envVars.end(); ++it) {
			// std::cout << "env entry is " << it->first << " " << it->second << std::endl;
			std::string envEntry = it->first + "=" + it->second;
			envArray[i] = new char[envEntry.size() + 1];
			std::strcpy(envArray[i], envEntry.c_str());
			i++;
		}
        envArray[i] = NULL;
        return envArray;
    }

void Cgi::execute(Response &response, Server &server, const Request &request) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        throw std::runtime_error("Failed to create pipe");
    }

    pid_t pid = fork();
    if (pid < 0) {
        throw std::runtime_error("Failed to fork");
    } else if (pid == 0) {
        // Child process: Execute the CGI script
        close(pipefd[0]);  // Close read end in child process

        // If the method is POST, redirect stdin to read from the pipe
        if (method == "POST") {
            int inputPipe[2];
            if (pipe(inputPipe) == -1) {
                perror("Failed to create input pipe");
                exit(EXIT_FAILURE);
            }

            // Write POST data to the input pipe
            ssize_t written = write(inputPipe[1], body.c_str(), body.size());
            close(inputPipe[1]);  // Close write end after writing
            if (written != static_cast<ssize_t>(body.size())) {
                perror("Failed to write all POST data to the CGI script");
            }
			//std::cout << "body is " << body.c_str() << std::endl;

            // Redirect stdin to read from the input pipe
            dup2(inputPipe[0], STDIN_FILENO);
            close(inputPipe[0]);  // Close read end of input pipe after dup2
        }

        // Redirect stdout to write to the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);  // Close write end of pipe after dup2


        // Build arguments and environment for execve
        char *args[] = {const_cast<char*>(path_info.c_str()), NULL};
        char **envArray = buildEnvArray();

        execve(path_info.c_str(), args, envArray);

        // If execve fails
        perror("execve failed");
        exit(EXIT_FAILURE);  // Exit child process on failure
    } else {
        // Parent process: Read the output of the CGI script
        close(pipefd[1]);  // Close write end in the parent process

        char buffer[1024];
        std::string result;
        ssize_t bytesRead;

        // Read from the pipe, which contains the CGI output
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            result.append(buffer, bytesRead);
        }
		// std::cout << "result is " << result << std::endl;
        close(pipefd[0]);  // Close read end after reading

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            std::cerr << "CGI script exited with status " << WEXITSTATUS(status) << std::endl;
            throw std::runtime_error("CGI script execution failed");
        }

		// std::cout << "result before check " << result << std::endl;
		if (check_correct_header(result, response, server, request)) {
			response.setStatusCode(200);
			response.setStatusMessage("OK");
			response.setBodyFromString(getBodyFromResponse(result));
		}
    }
}



bool Cgi::check_correct_header(std::string &result, Response &response, Server &server,const Request &request) {
    // Check if the result is empty
	// std::cout << "result in check_correct header is: '" << result << "'" << std::endl;
    // std::cout << "result length: " << result.length() << std::endl;
    if (result.empty()) {
		response.setStatusCode(500);
		response.setStatusMessage("Internal Server Error");
		response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
		return false;
    }

    // Check for headers
    // std::cout << "result in correct header is " << result << std::endl;

    // Find the "Status" header
    size_t statusPos = result.find("Status: ");
    if (statusPos == std::string::npos) {
        response.setStatusCode(500);
		response.setStatusMessage("Internal Server Error");
		response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
		return false;
    }

    // Extract the status line
    size_t statusEnd = result.find("\r\n", statusPos);
    if (statusEnd == std::string::npos) {
        response.setStatusCode(500);
		response.setStatusMessage("Internal Server Error");
		response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
		return false;
    }

    std::string statusLine = result.substr(statusPos, statusEnd - statusPos);
	// std::cout << "status line is " << statusLine << std::endl;
    // Check if the status is "200 OK"
    if (statusLine.find("200 OK") == std::string::npos) {
        response.setStatusCode(500);
		response.setStatusMessage("Internal Server Error");
		response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
		return false;
    }

	std::string contentType = result.substr(result.find("Content-Type: ") + 14);
	contentType = contentType.substr(0, contentType.find("\r\n"));
	// response.setHeader("Content-Type", contentType);

	// std::cout << "content type in check headers of the response  is |" << contentType << "|" << std::endl;
	// std::cout << std::endl;
	// std::cout << "request content type is |" << request.getContentType() << "|"<<std::endl;

	// Check if the content type matches the request only for POST requests because GET requests can have any content type
	if (request.getMethod() == "POST" &&  contentType != request.getContentType()) {
		response.setStatusCode(500);
		response.setStatusMessage("Internal Server Error");
		response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
		return false;
	}
    return true;
}

std::string Cgi::getBodyFromResponse(const std::string& response) {
    std::string::size_type pos = response.find("\r\n\r\n");

    if (pos != std::string::npos) {
        return response.substr(pos + 4);
    } else {
        // If there's no header or if it's not formatted correctly, return the whole response
        return response;
    }
}


void Cgi::extract_script_name(const std::string &path){
    size_t lastSlashPos = path.find_last_of('/');
    size_t queryPos = path.find('?');

    if (lastSlashPos != std::string::npos)
    {
        if (queryPos != std::string::npos)
        {
            // If there's a query string, extract the part between the last slash and the question mark
            script_name = path.substr(lastSlashPos + 1, queryPos - lastSlashPos - 1);
        }
        else
        {
            // If there's no query string, extract the part after the last slash
            script_name = path.substr(lastSlashPos + 1);
        }
    }
    else
    {
        script_name = path;
    }
}

// std::string Cgi::handleCgiRequest(Cgi &cgi, const std::string &method, const std::string &body, const std::string &contentType)
// {
// 	// std::cout<< cgi.getPath_info() << std::endl;
// 	// extract_query_string(path); // not necessary anymore
//     // std::string postBody = "name=John&age=30";
//     if (cgi.isCgiRequest(cgi.getPath_info())) {
// 	// 	//set the method of the HTTP
//         cgi.setMethod(method);
//         // // Extract path info and query string


// 		// //set the script name
// 		// setScriptName(scriptName);
//         // if (scriptName.empty()) {
//         //     return cgi.generateErrorResponse(404, "Not Found", "No script specified");
//         // }

//         // // Check if the script has execute permission
//         if (access(cgi.getPath_info().c_str(), X_OK) == -1) {
//             std::cerr << "Script has no execute permission: " << cgi.getPath_info().c_str() << std::endl;
//             return cgi.generateErrorResponse(403, "Forbidden", "Script has no execute permission");
//         }

// 		// //now we can set the path_info to use the fullScrpiptPath
//         // cgi.setPath_info(fullScriptPath);

//         cgi.prepareEnvVars( body, contentType);
// 		// Execute the CGI script and get the output
// 		try {
// 			std::string getResponse = cgi.execute();
// 			// std::cout << getResponse << std::endl;
// 			return getResponse;
// 		} catch (const std::exception &e) {
// 			std::cerr << "Error executing CGI script: " << e.what() << std::endl;
// 			return cgi.generateErrorResponse(500, "Internal Server Error", "Error executing CGI script");
// 		}
// 	}
// 	return cgi.generateErrorResponse(404, "Not Found", "Not a CGI request");
// }
