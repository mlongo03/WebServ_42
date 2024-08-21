#include "Cgi.hpp"
#include <limits.h>

    // Constructor
   Cgi::Cgi(const std::string &cgiBinPath, const std::string &cgiRootPath, const std::vector<std::string> &extensions)
        : cgiBin(cgiBinPath), cgiRoot(cgiRootPath), cgiExtensions(extensions) {}

    // Setters for method, script path, and request body
    void Cgi::setMethod(const std::string &httpMethod) {
        method = httpMethod;
    }

    void Cgi::setScriptPath(const std::string &path) {
        scriptPath = cgiRoot + path;
		scriptPath = makeRelativePath(scriptPath); // Remove leading '/'
		// std::cout << "script path is " << scriptPath << std::endl;
    }

    void Cgi::setBody(const std::string &requestBody) {
        body = requestBody;
    }

    // Check if the requested URL matches a CGI request
 bool Cgi::isCgiRequest(const std::string &url) {
    std::string scriptPath = url.substr(0, url.find('?')); // Remove query string if present
    std::string scriptExtension = scriptPath.substr(scriptPath.find_last_of("."));

    for (std::vector<std::string>::const_iterator it = cgiExtensions.begin(); it != cgiExtensions.end(); ++it) {
        if (scriptExtension == *it) {
            return true;
        }
    }
    return false;
}

void Cgi::prepareEnvVars(const std::string &queryString) {
    envVars["REQUEST_METHOD"] = method;
    envVars["SCRIPT_NAME"] = scriptPath;
    envVars["QUERY_STRING"] = queryString;
    envVars["PATH_INFO"] = scriptPath;

    if (method == "POST") {
        std::ostringstream oss;
        oss << body.size();
		std::cout << "content length in post is " << oss.str() << std::endl;
        envVars["CONTENT_LENGTH"] = oss.str();
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

std::string Cgi::execute() {
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
			std::cout << "body is " << body.c_str() << std::endl;

            // Redirect stdin to read from the input pipe
            dup2(inputPipe[0], STDIN_FILENO);
            close(inputPipe[0]);  // Close read end of input pipe after dup2
        }

        // Redirect stdout to write to the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);  // Close write end of pipe after dup2

        // Build arguments and environment for execve
        char *args[] = {const_cast<char*>(scriptPath.c_str()), NULL};
        char **envArray = buildEnvArray();

        execve(scriptPath.c_str(), args, envArray);

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
		return check_correct_header(result);
        return result;
    }
}



std::string Cgi::makeRelativePath(const std::string &path) {
    if (!path.empty() && path[0] == '/') {
        return path.substr(1);  // Remove the leading '/'
    }
    return path;  // Return the path as-is if it doesn't start with '/'
}

std::string Cgi::check_correct_header(std::string &result) {
    // Check if the result is empty
	// std::cout << "result in check_correct header is: '" << result << "'" << std::endl;
    // std::cout << "result length: " << result.length() << std::endl;
    if (result.empty()) {
        return generateErrorResponse(500, "Internal Server Error", "CGI script did not produce correct headers.");
    }

    // Check for headers
    // std::cout << "result in correct header is " << result << std::endl;

    // Find the "Status" header
    size_t statusPos = result.find("Status: ");
    if (statusPos == std::string::npos) {
        return generateErrorResponse(500, "Internal Server Error", "CGI script did not produce correct headers.");
    }

    // Extract the status line
    size_t statusEnd = result.find("\r\n", statusPos);
    if (statusEnd == std::string::npos) {
        return generateErrorResponse(500, "Internal Server Error", "CGI script did not produce correct headers.");
    }

    std::string statusLine = result.substr(statusPos, statusEnd - statusPos);
	// std::cout << "status line is " << statusLine << std::endl;
    // Check if the status is "200 OK"
    if (statusLine.find("200 OK") == std::string::npos) {
        return generateErrorResponse(500, "Internal Server Error", "CGI script did not produce correct headers.");
    }

    return result;
}

std::string Cgi::generateErrorResponse(int statusCode, const std::string& statusMessage, const std::string& errorMessage) {
    // Construct the error body
    std::stringstream ss;
    ss << "<html><body><h1>" << statusCode << " " << statusMessage << "</h1>"
       << "<p>" << errorMessage << "</p></body></html>";
    std::string errorBody = ss.str();

    // Convert the content length to a string using stringstream
    std::stringstream contentLengthStream;
    contentLengthStream << errorBody.size();
    std::string contentLengthStr = contentLengthStream.str();

    // Construct the error response
    std::stringstream responseStream;
    responseStream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n"
                   << "Content-Type: text/html\r\n"
                   << "Content-Length: " << contentLengthStr << "\r\n\r\n"
                   << errorBody;
    return responseStream.str();
}
