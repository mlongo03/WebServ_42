#include "cgi.hpp"
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
        envVars["CONTENT_LENGTH"] = oss.str();
    }
}

    // Prepare environment variables for the CGI script
    // void Cgi::prepareEnvVars(const std::string &pathInfo) {
    //     envVars["REQUEST_METHOD"] = method;
    //     envVars["SCRIPT_NAME"] = scriptPath;
    //     envVars["PATH_INFO"] = pathInfo;
	// 	// if (method == "POST") {
	// 	// 	std::ostringstream oss;
	// 	// 	oss << body.size();
	// 	// 	envVars["CONTENT_LENGTH"] = oss.str();
	// 	// }
	// 	if (method == "GET") {
	// 		// Extract query string from pathInfo if it exists
	// 		std::cout << pathInfo << std::endl;
	// 		std::size_t queryPos = pathInfo.find("?");
	// 		if (queryPos != std::string::npos) {
	// 			envVars["QUERY_STRING"] = pathInfo.substr(queryPos + 1);
	// 		} else {
	// 			envVars["QUERY_STRING"] = ""; // No query string present
	// 		}
	// 	}
	// 	if (method == "POST")
	// 	{
	// 		std::ostringstream oss;
	// 		oss << body.size();
	// 		envVars["CONTENT_LENGTH"] = oss.str();
	// 		std::cout << "content length is " << oss.str() << std::endl;
	// 	}
	// 		// Add more environment variables as needed...
	// 	}

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

    // // Execute the CGI script
    // std::string Cgi::execute() {
    //     int pipefd[2];
    //     if (pipe(pipefd) == -1) {
    //         throw std::runtime_error("Failed to create pipe");
    //     }

    //     pid_t pid = fork();
    //     if (pid < 0) {
    //         throw std::runtime_error("Failed to fork");
    //     } else if (pid == 0) {
    //         // Child process: Execute the CGI script
    //         close(pipefd[0]);  // Close read end

    //         dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to the pipe
    //         close(pipefd[1]);

    //         // Build arguments and environment
    //         char *args[] = {const_cast<char*>(scriptPath.c_str()), NULL};
    //         char **envArray = buildEnvArray();

    //         execve(scriptPath.c_str(), args, envArray);

    //         // If execve fails
    //         perror("execve");
    //         exit(EXIT_FAILURE);
    //     } else {
    //         // Parent process: Read the output of the CGI script
    //         close(pipefd[1]);  // Close write end
    //         char buffer[1024];
    //         std::string result;
    //         ssize_t bytesRead;
    //         while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
    //             result.append(buffer, bytesRead);
    //         }
    //         close(pipefd[0]);

    //         // Wait for the child process to finish
    //         int status;
    //         waitpid(pid, &status, 0);

    //         return result;
    //     }
    // }

// std::string Cgi::execute() {
//     int pipefd[2];
//     if (pipe(pipefd) == -1) {
//         throw std::runtime_error("Failed to create pipe");
//     }

//     pid_t pid = fork();
//     if (pid < 0) {
//         throw std::runtime_error("Failed to fork");
//     } else if (pid == 0) {
//         // Child process: Execute the CGI script
//         close(pipefd[0]);  // Close read end

//         dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to the pipe
//         close(pipefd[1]);

//         // Build arguments and environment
//         char *args[] = {const_cast<char*>(scriptPath.c_str()), NULL};
//         char **envArray = buildEnvArray();

//         // Debugging: Print information before execve
//         // std::cerr << "Executing script.c_str(): " << scriptPath.c_str() << std::endl;
// 		// char cwd[PATH_MAX];
// 		// if (getcwd(cwd, sizeof(cwd)) == NULL) {
// 		// 	// std::cout << "pwd is " << cwd << std::endl;
// 		// 	throw std::runtime_error("Failed to get current working directory");
// 		// }
// 		// std::cerr << "Current working directory: " << cwd << std::endl;

//         execve(scriptPath.c_str(), args, envArray);

//         // If execve fails
//         perror("execve");
//         exit(EXIT_FAILURE);
//     } else {
//         // Parent process: Read the output of the CGI script
//         close(pipefd[1]);  // Close write end
//         char buffer[1024];
//         std::string result;
//         ssize_t bytesRead;
//         while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
//             result.append(buffer, bytesRead);
//         }
//         close(pipefd[0]);

//         // Wait for the child process to finish
//         int status;
//         waitpid(pid, &status, 0);

//         return result;
//     }
// }

std::string Cgi::execute() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        throw std::runtime_error("Failed to create pipe");
    }

    pid_t pid = fork();
    if (pid < 0) {
        throw std::runtime_error("Failed to fork");


    }
	else if (pid == 0) {
        // Child process: Execute the CGI script
        close(pipefd[0]);  // Close read end

        // If the method is POST, write the body (POST data) to the pipe
		if (method == "POST") {
		ssize_t written = write(pipefd[1], body.c_str(), body.size());
		if (written != static_cast<ssize_t>(body.size())) {
			perror("Failed to write all POST data to the CGI script");
		}
		}
        // Important: Close write end after writing the POST data
        close(pipefd[1]);

        // Build arguments and environment for execve
        char *args[] = {const_cast<char*>(scriptPath.c_str()), NULL};
        char **envArray = buildEnvArray();

        // Debugging output for execve
        std::cerr << "Executing CGI script: " << scriptPath << std::endl;
        for (char **env = envArray; *env != NULL; ++env) {
            std::cerr << "Environment: " << *env << std::endl;
        }

        execve(scriptPath.c_str(), args, envArray);

        // If execve fails
        perror("execve failed");
        close(pipefd[1]);
        exit(EXIT_FAILURE);  // Exit child process on failure


    } else {
        // Parent process: Read the output of the CGI script
        close(pipefd[1]);  // Close write end in the parent

        char buffer[1024];
        std::string result;
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            result.append(buffer, bytesRead);
        }
        close(pipefd[0]);  // Close read end after reading

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        // Check if child process exited successfully
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            std::cerr << "CGI script exited with status " << WEXITSTATUS(status) << std::endl;
            throw std::runtime_error("CGI script execution failed");
        }

        return result;
    }
}



std::string Cgi::makeRelativePath(const std::string &path) {
    if (!path.empty() && path[0] == '/') {
        return path.substr(1);  // Remove the leading '/'
    }
    return path;  // Return the path as-is if it doesn't start with '/'
}
