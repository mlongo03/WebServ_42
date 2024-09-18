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
        body = request.getBody();
    }
}

    // Convert map of environment variables to char* array for execve
    char **Cgi::buildEnvArray() {
        char **envArray = new char*[envVars.size() + 1];
        int i = 0;
		for (std::map<std::string, std::string>::const_iterator it = envVars.begin(); it != envVars.end(); ++it) {
			std::string envEntry = it->first + "=" + it->second;
			envArray[i] = new char[envEntry.size() + 1];
			std::strcpy(envArray[i], envEntry.c_str());
			i++;
		}
        envArray[i] = NULL;
        return envArray;
    }

// void Cgi::execute(Response &response, Server &server, const Request &request) {
//     int pipefd[2];
//     if (pipe(pipefd) == -1) {
//         throw std::runtime_error("Failed to create pipe");
//     }

//     pid_t pid = fork();
//     if (pid < 0) {
//         throw std::runtime_error("Failed to fork");
//     } else if (pid == 0) {
//         // Child process: Execute the CGI script
//         close(pipefd[0]);  // Close read end in child process

// 		// // Set an alarm for the child process
//         // alarm(3);  // Set a timeout of 3 seconds


//         // If the method is POST, redirect stdin to read from the pipe
//         if (method == "POST") {
//             int inputPipe[2];
//             if (pipe(inputPipe) == -1) {
//                 perror("Failed to create input pipe");
//                 exit(EXIT_FAILURE);
//             }

//             // Write POST data to the input pipe
//             ssize_t written = write(inputPipe[1], body.c_str(), body.size());
//             close(inputPipe[1]);  // Close write end after writing
//             if (written != static_cast<ssize_t>(body.size())) {
//                 perror("Failed to write all POST data to the CGI script");
//             }
// 			//std::cout << "body is " << body.c_str() << std::endl;

//             // Redirect stdin to read from the input pipe
//             dup2(inputPipe[0], STDIN_FILENO);
//             close(inputPipe[0]);  // Close read end of input pipe after dup2
//         }

//         // Redirect stdout to write to the pipe
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[1]);  // Close write end of pipe after dup2


//         // Build arguments and environment for execve
//         char *args[] = {const_cast<char*>(path_info.c_str()), NULL};
//         char **envArray = buildEnvArray();

//         execve(path_info.c_str(), args, envArray);

//         // If execve fails
//         perror("execve failed");
//         exit(EXIT_FAILURE);  // Exit child process on failure
//     } else {
//         // Parent process: Read the output of the CGI script
//         close(pipefd[1]);  // Close write end in the parent process

//         char buffer[1024];
//         std::string result;
//         ssize_t bytesRead;

//         // Read from the pipe, which contains the CGI output
//         while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
//             result.append(buffer, bytesRead);
//         }
// 		// std::cout << "result is " << result << std::endl;
//         close(pipefd[0]);  // Close read end after reading

//         // Wait for the child process to finish
//         int status;
//         waitpid(pid, &status, 0);

// 		// if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM) {
//         //     std::cerr << "CGI script timed out" << std::endl;
//         //     throw std::runtime_error("CGI script execution timed out");
//         // }

//         if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
//             response.setStatusCode(400);
// 			response.setStatusMessage("Bad Request");
// 			response.setBodyFromFile(server.getRoot() + server.getErrorPage400());
//         }

// 		if (check_correct_header(result, response, server, request)) {
// 			response.setStatusCode(200);
// 			response.setStatusMessage("OK");
// 			response.setBodyFromString(getBodyFromResponse(result));
// 		}
//     }
// }

// #include <sys/epoll.h>
// #include <sys/wait.h>
// #include <unistd.h>
// #include <stdexcept>
// #include <cstring>
// #include <iostream>

// void Cgi::execute(Response &response, Server &server, const Request &request) {
//     int pipefd[2];
//     if (pipe(pipefd) == -1) {
//         throw std::runtime_error("Failed to create pipe");
//     }

//     pid_t pid = fork();
//     if (pid < 0) {
//         throw std::runtime_error("Failed to fork");
//     } else if (pid == 0) {
//         // Child process: Execute the CGI script
//         close(pipefd[0]);  // Close read end in child process

//         // If the method is POST, redirect stdin to read from the pipe
//         if (method == "POST") {
//             int inputPipe[2];
//             if (pipe(inputPipe) == -1) {
//                 perror("Failed to create input pipe");
//                 exit(EXIT_FAILURE);
//             }

//             // Write POST data to the input pipe
//             ssize_t written = write(inputPipe[1], body.c_str(), body.size());
//             close(inputPipe[1]);  // Close write end after writing
//             if (written != static_cast<ssize_t>(body.size())) {
//                 perror("Failed to write all POST data to the CGI script");
//             }

//             // Redirect stdin to read from the input pipe
//             dup2(inputPipe[0], STDIN_FILENO);
//             close(inputPipe[0]);  // Close read end of input pipe after dup2
//         }

//         // Redirect stdout to write to the pipe
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[1]);  // Close write end of pipe after dup2

//         // Build arguments and environment for execve
//         char *args[] = {const_cast<char*>(path_info.c_str()), NULL};
//         char **envArray = buildEnvArray();

//         execve(path_info.c_str(), args, envArray);

//         // If execve fails
//         perror("execve failed");
//         exit(EXIT_FAILURE);  // Exit child process on failure
//     } else {
//         // Parent process: Read the output of the CGI script
//         close(pipefd[1]);  // Close write end in the parent process

//         // Create an epoll instance
//         int epoll_fd = epoll_create1(0);
//         if (epoll_fd == -1) {
//             throw std::runtime_error("Failed to create epoll instance");
//         }

//         // Add the pipe read end to the epoll instance
//         struct epoll_event event;
//         event.events = EPOLLIN;
//         event.data.fd = pipefd[0];
//         if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipefd[0], &event) == -1) {
//             close(epoll_fd);
//             throw std::runtime_error("Failed to add file descriptor to epoll");
//         }

//         // Set a timeout for the epoll wait
//         int timeout = 3000;  // Timeout in milliseconds (3 seconds)
//         struct epoll_event events[1];
//         int nfds = epoll_wait(epoll_fd, events, 1, timeout);

//         if (nfds == -1) {
//             close(epoll_fd);
//             throw std::runtime_error("epoll_wait failed");
//         } else if (nfds == 0) {
//             // Timeout occurred, kill the child process
//             kill(pid, SIGKILL);
//             close(epoll_fd);
//             throw std::runtime_error("CGI script execution timed out");
//         }

//         // Read from the pipe, which contains the CGI output
//         char buffer[1024];
//         std::string result;
//         ssize_t bytesRead;
//         while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
//             result.append(buffer, bytesRead);
//         }
//         close(pipefd[0]);  // Close read end after reading

//         // Wait for the child process to finish
//         int status;
//         waitpid(pid, &status, 0);

//         if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
//             response.setStatusCode(400);
//             response.setStatusMessage("Bad Request");
//             response.setBodyFromFile(server.getRoot() + server.getErrorPage400());
//         }

//         if (check_correct_header(result, response, server, request)) {
//             response.setStatusCode(200);
//             response.setStatusMessage("OK");
//             response.setBodyFromString(getBodyFromResponse(result));
//         }

//         close(epoll_fd);  // Close the epoll instance
//     }
// }

#include <sys/epoll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <ctime>

void Cgi::execute(Response &response, Server &server, const Request &request) {
    const int timeoutDuration = 3; // Timeout duration in seconds

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

        // Create an epoll instance
        int epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
            throw std::runtime_error("Failed to create epoll instance");
        }

        // Add the pipe read end to the epoll instance
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = pipefd[0];
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipefd[0], &event) == -1) {
            close(epoll_fd);
            throw std::runtime_error("Failed to add file descriptor to epoll");
        }

        // Get the start time
        time_t startTime = std::time(NULL);

        // Set a timeout for the epoll wait
        struct epoll_event events[1];
        bool timeoutOccurred = false;
        std::string result;

        while (true) {
            int nfds = epoll_wait(epoll_fd, events, 1, 1000);  // 1-second epoll wait
            if (nfds == -1) {
                close(epoll_fd);
                throw std::runtime_error("epoll_wait failed");
            } else if (nfds == 0) {
                // Check for timeout
                time_t currentTime = std::time(NULL);
                double elapsedTime = std::difftime(currentTime, startTime);
                if (elapsedTime > timeoutDuration) {
                    // Timeout occurred, kill the child process
                    std::cerr << "CGI script timed out" << std::endl;
                    kill(pid, SIGKILL);
                    timeoutOccurred = true;
                    break;
                }
                continue;
            }

            // Read from the pipe, which contains the CGI output
            char buffer[1024];
            ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer));
            if (bytesRead > 0) {
                result.append(buffer, bytesRead);
            } else if (bytesRead == 0) {
                // End of file, child process has finished
                break;
            } else {
                // Error reading from pipe
                close(epoll_fd);
                throw std::runtime_error("Error reading from pipe");
            }
        }

        close(pipefd[0]);  // Close read end after reading

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (timeoutOccurred || (WIFEXITED(status) && WEXITSTATUS(status) != 0)) {
            response.setStatusCode(400);
            response.setStatusMessage("Bad Request");
            response.setBodyFromFile(server.getRoot() + server.getErrorPage400());
        } else if (check_correct_header(result, response, server, request)) {
            response.setStatusCode(200);
            response.setStatusMessage("OK");
            response.setBodyFromString(getBodyFromResponse(result));
        } else {
            response.setStatusCode(500);
            response.setStatusMessage("Internal Server Error");
            response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
        }

        close(epoll_fd);  // Close the epoll instance
    }
}

// #include <sys/epoll.h>
// #include <sys/wait.h>
// #include <unistd.h>
// #include <stdexcept>
// #include <cstring>
// #include <iostream>

// void Cgi::execute(Response &response, Server &server, const Request &request) {
//     int pipefd[2];
//     if (pipe(pipefd) == -1) {
//         throw std::runtime_error("Failed to create pipe");
//     }

//     pid_t pid = fork();
//     if (pid < 0) {
//         throw std::runtime_error("Failed to fork");
//     } else if (pid == 0) {
//         // Child process: Execute the CGI script
//         close(pipefd[0]);  // Close read end in child process

//         // If the method is POST, redirect stdin to read from the pipe
//         if (method == "POST") {
//             int inputPipe[2];
//             if (pipe(inputPipe) == -1) {
//                 perror("Failed to create input pipe");
//                 exit(EXIT_FAILURE);
//             }

//             // Write POST data to the input pipe
//             ssize_t written = write(inputPipe[1], body.c_str(), body.size());
//             close(inputPipe[1]);  // Close write end after writing
//             if (written != static_cast<ssize_t>(body.size())) {
//                 perror("Failed to write all POST data to the CGI script");
//             }

//             // Redirect stdin to read from the input pipe
//             dup2(inputPipe[0], STDIN_FILENO);
//             close(inputPipe[0]);  // Close read end of input pipe after dup2
//         }

//         // Redirect stdout to write to the pipe
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[1]);  // Close write end of pipe after dup2

//         // Build arguments and environment for execve
//         char *args[] = {const_cast<char*>(path_info.c_str()), NULL};
//         char **envArray = buildEnvArray();

//         execve(path_info.c_str(), args, envArray);

//         // If execve fails
//         perror("execve failed");
//         exit(EXIT_FAILURE);  // Exit child process on failure
//     } else {
//         // Parent process: Read the output of the CGI script
//         close(pipefd[1]);  // Close write end in the parent process

//         // Create an epoll instance
//         int epoll_fd = epoll_create1(0);
//         if (epoll_fd == -1) {
//             throw std::runtime_error("Failed to create epoll instance");
//         }

//         // Add the pipe read end to the epoll instance
//         struct epoll_event event;
//         event.events = EPOLLIN;
//         event.data.fd = pipefd[0];
//         if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipefd[0], &event) == -1) {
//             close(epoll_fd);
//             throw std::runtime_error("Failed to add file descriptor to epoll");
//         }

//         // Set a timeout for the epoll wait
//         int timeout = 3000;  // Timeout in milliseconds (3 seconds)
//         struct epoll_event events[1];
//         int nfds = epoll_wait(epoll_fd, events, 1, timeout);
// 		std::cout << "nfds value is = " << nfds << std::endl;
//         if (nfds == -1) {
//             close(epoll_fd);
//             throw std::runtime_error("epoll_wait failed");
//         } else if (nfds == 0) {
//             // Timeout occurred, kill the child process
// 			std::cerr << "CGI script timed out" << std::endl;
//             kill(pid, SIGKILL);
//             close(epoll_fd);
//             throw std::runtime_error("CGI script execution timed out");
//         }

//         // Read from the pipe, which contains the CGI output
//         char buffer[1024];
//         std::string result;
//         ssize_t bytesRead;
//         while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
//             result.append(buffer, bytesRead);
//         }
//         close(pipefd[0]);  // Close read end after reading

//         // Wait for the child process to finish
//         int status;
//         waitpid(pid, &status, 0);

//         if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
//             response.setStatusCode(500);
//             response.setStatusMessage("Internal Server Error");
//             response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
//         }

//         if (check_correct_header(result, response, server, request)) {
//             response.setStatusCode(200);
//             response.setStatusMessage("OK");
//             response.setBodyFromString(getBodyFromResponse(result));
//         }

//         close(epoll_fd);  // Close the epoll instance
//     }
// }

// #include <sys/epoll.h>
// #include <sys/wait.h>
// #include <unistd.h>
// #include <stdexcept>
// #include <cstring>
// #include <iostream>

// void Cgi::execute(Response &response, Server &server, const Request &request) {
//     int pipefd[2];
//     if (pipe(pipefd) == -1) {
//         throw std::runtime_error("Failed to create pipe");
//     }

//     pid_t pid = fork();
//     if (pid < 0) {
//         throw std::runtime_error("Failed to fork");
//     } else if (pid == 0) {
//         // Child process: Execute the CGI script
//         close(pipefd[0]);  // Close read end in child process

//         // If the method is POST, redirect stdin to read from the pipe
//         if (method == "POST") {
//             int inputPipe[2];
//             if (pipe(inputPipe) == -1) {
//                 perror("Failed to create input pipe");
//                 exit(EXIT_FAILURE);
//             }

//             // Write POST data to the input pipe
//             ssize_t written = write(inputPipe[1], body.c_str(), body.size());
//             close(inputPipe[1]);  // Close write end after writing
//             if (written != static_cast<ssize_t>(body.size())) {
//                 perror("Failed to write all POST data to the CGI script");
//             }

//             // Redirect stdin to read from the input pipe
//             dup2(inputPipe[0], STDIN_FILENO);
//             close(inputPipe[0]);  // Close read end of input pipe after dup2
//         }

//         // Redirect stdout to write to the pipe
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[1]);  // Close write end of pipe after dup2

//         // Build arguments and environment for execve
//         char *args[] = {const_cast<char*>(path_info.c_str()), NULL};
//         char **envArray = buildEnvArray();

//         execve(path_info.c_str(), args, envArray);

//         // If execve fails
//         perror("execve failed");
//         exit(EXIT_FAILURE);  // Exit child process on failure
//     } else {
//         // Parent process: Read the output of the CGI script
//         close(pipefd[1]);  // Close write end in the parent process

//         // Create an epoll instance
//         int epoll_fd = epoll_create1(0);
//         if (epoll_fd == -1) {
//             throw std::runtime_error("Failed to create epoll instance");
//         }

//         // Add the pipe read end to the epoll instance
//         struct epoll_event event;
//         event.events = EPOLLIN;
//         event.data.fd = pipefd[0];
//         if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipefd[0], &event) == -1) {
//             close(epoll_fd);
//             throw std::runtime_error("Failed to add file descriptor to epoll");
//         }

//         // Set a timeout for the epoll wait
//         int timeout = 3000;  // Timeout in milliseconds (3 seconds)
//         struct epoll_event events[1];
//         int nfds = epoll_wait(epoll_fd, events, 1, timeout);

//         if (nfds == -1) {
//             close(epoll_fd);
//             throw std::runtime_error("epoll_wait failed");
//         } else if (nfds == 0) {
//             // Timeout occurred, kill the child process
//             kill(pid, SIGKILL);
//             close(epoll_fd);
//             throw std::runtime_error("CGI script execution timed out");
//         }

//         // Read from the pipe, which contains the CGI output
//         char buffer[1024];
//         std::string result;
//         ssize_t bytesRead;
//         while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
//             result.append(buffer, bytesRead);
//         }
//         close(pipefd[0]);  // Close read end after reading

//         // Wait for the child process to finish
//         int status;
//         pid_t waitResult = waitpid(pid, &status, WNOHANG);
//         if (waitResult == 0) {
//             // Child process is still running, wait with a timeout
//             struct timespec ts;
//             ts.tv_sec = 3;  // 3 seconds timeout
//             ts.tv_nsec = 0;
//             int s = nanosleep(&ts, NULL);
//             if (s == -1) {
//                 perror("nanosleep");
//             }
//             // Check again if the child process has finished
//             waitResult = waitpid(pid, &status, WNOHANG);
//             if (waitResult == 0) {
//                 // Child process is still running, kill it
//                 kill(pid, SIGKILL);
//                 waitpid(pid, &status, 0);  // Wait for the child process to terminate
//                 response.setStatusCode(500);
//                 response.setStatusMessage("Internal Server Error");
//                 response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
//             }
//         }

//         if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
//             response.setStatusCode(500);
//             response.setStatusMessage("Internal Server Error");
//             response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
//         }

//         if (check_correct_header(result, response, server, request)) {
//             response.setStatusCode(200);
//             response.setStatusMessage("OK");
//             response.setBodyFromString(getBodyFromResponse(result));
//         }

//         close(epoll_fd);  // Close the epoll instance
//     }
// }

bool Cgi::check_correct_header(std::string &result, Response &response, Server &server,const Request &request) {
    if (result.empty()) {
		response.setStatusCode(500);
		response.setStatusMessage("Internal Server Error");
		response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
		return false;
    }

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
    if (statusLine.find("200 OK") == std::string::npos) {
        response.setStatusCode(500);
		response.setStatusMessage("Internal Server Error");
		response.setBodyFromFile(server.getRoot() + server.getErrorPage500());
		return false;
    }

	std::string contentType = result.substr(result.find("Content-Type: ") + 14);
	contentType = contentType.substr(0, contentType.find("\r\n"));
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
