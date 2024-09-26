# WebServer

Welcome to the **WebServer** project! This is a fully functional HTTP web server built in C++98, supporting essential web server features like **GET**, **POST**, and **DELETE** requests, **CGI** handling, virtual hosts, and more.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Testing](#testing)
- [Dependencies](#dependencies)
- [Project Structure](#project-structure)
- [License](#license)

## Features

- **HTTP/1.1 Compliance**: Supports basic HTTP request methods (GET, POST, DELETE).
- **Virtual Hosts**: Multiple server configurations on the same IP and port, distinguished by the hostname.
- **CGI Support**: Handles CGI scripts for dynamic content generation (e.g., PHP, Python).
- **Static File Serving**: Serves static files from specified directories.
- **Error Pages**: Custom error pages for 404, 403, 500 errors, etc.
- **Chunked Transfer Encoding**: Properly handles chunked data in requests.
- **Cookies** and session management (see below for more information)
- **Multithreaded/Multiprocess**: Can handle concurrent connections using epoll (or other async mechanisms).
- **Logging**: Provides basic logs for connections and errors.
- **Stress-Tested**: Battle-tested with tools like `siege` for stability and performance.

## Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/webserver.git
   cd webserver
   ```

2. **Build the project**:
   - The project is built with `make`. Ensure you have `g++` and `make` installed on your system.
   ```bash
   make
   ```

3. **Run the server**:
   ```bash
   ./webserv [configuration file]
   ```

   Example configuration file provided as `config/default.conf`.

## Usage

Once the server is running, you can make HTTP requests to it via a web browser, `curl`, or automated testing tools like `siege`.

### Example `curl` requests:
- **GET request**:
  ```bash
  curl http://localhost:8080/index.html
  ```

- **POST request**:
  ```bash
  curl -X POST -d "name=test" http://localhost:8080/form-handler
  ```

- **DELETE request**:
  ```bash
  curl -X DELETE http://localhost:8080/delete-file
  ```

## Configuration

The server can be configured using a configuration file in a similar format to Nginx. Example configuration:

```conf
server {
	server_name test.com ciao.com;

	listen 3031;
	root /var/www;
	host 192.168.122.1;
	client_max_body_size 1000;
	cgi_extension .py;

	autoindex    on;

    location /get_test {
        index index.html;
        allow GET;
	}

	location /old-page {
		allow GET DELETE;
		return 302 /new-page/;
	}
}
```

Key sections:
- **listen**: Port the server listens on.
- **host**: Ip address the server listens on.
- **server_name**: Hostname for virtual hosting.
- **root**: Directory to serve static files.
- **index**: Default file to serve (e.g., `index.html`).
- **cgi_extension**: define file extensions that the cgi can execute (.py, .pl, .sh).
- **error_page**: Custom error pages for different status codes.
- **allow**: define Methods accepted (GET, POST, DELETE).
- **autoindex**: set the autoindex (on, off).
- **upload_dir**: can be set only in routes (upload of files can be done only in routes) and set the path where the file should be created.
- **return**: set the redirection page and the associated status code.
- **client_max_body_size**: set the max body size of a POST request and is expressed in kilobytes (kb).

## Testing

### Siege & Stress Testing
Run a **siege** test to verify server performance and stability:

```bash
siege -b http://localhost:8080
```

### Memory Leak Checking
Use `valgrind` to ensure no memory leaks:

```bash
valgrind --leak-check=full ./webserv config/default.conf
```

### Manual Testing
You can also manually test the server using a browser or `curl`. Make sure:
- Content is served correctly.
- No memory leaks occur.
- Connections close cleanly after requests (no hanging connections).

## Dependencies

- **C++98**: The project is built strictly with C++98 for compatibility reasons.
- **POSIX**: Some system calls like `epoll` and `fork` are used to manage concurrency and processes.

## Project Structure

```
webserver/
│
├── src/                    # folder containing c++ source code
│   ├── main.cpp            # Entry point of the server
│   ├── Server.cpp          # Contains information of a server configuration
│   ├── Request.cpp         # HTTP request handling
│   ├── Response.cpp        # HTTP response generation
│   ├── Cgi.cpp             # CGI handler
│   ├── ConfigParser.cpp    # Configuration file parser
│   ├── Worker.cpp          # Handles the main loop and so handle I/O operations
|   ├── Client.cpp          # Contains informations about a client, a tcp connection (e.g., request string, response string)
|   ├── Epoll.cpp           # Handles the logic behind the epoll() functions
|   ├── Socket.cpp          # Handle Sockets creation
|   ├── SignalHandler.cpp   # Handle the SIGINT signal 
|   ├── Location.cpp        # Contains information of a location in the server configuration
│   └── Utils.cpp           # Contains other functions utilities
|
├── include/                # folder containing headers
│   ├── Server.hpp          # Header for Server class
│   ├── Request.hpp         # Header for Request class
│   ├── Response.hpp        # Header for Response class
│   ├── Cgi.hpp             # Header for Cgi class
│   ├── ConfigParser.hpp    # Header for ConfigParser class
│   ├── Worker.hpp          # Header for Worker class
|   ├── Client.hpp          # Header for Client class
|   ├── Epoll.hpp           # Header for Epoll class
|   ├── Socket.hpp          # Header for Socket class
|   ├── SignalHandler.hpp   # Header for SignalHandler class 
|   ├── Location.hpp        # Header for Location class
|   ├── Exception.hpp       # Contains Exceptions
|   ├── Global.hpp          # Contains most of headers declarations shared in classes
│   └── Utils.cpp           # Header for Utils class
│
├── config/                 # folder containing configuration files 
│   ├── default.conf        # Example configuration file
│
├── Makefile                # Build instructions
└── README.md               # This README file
```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
