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
    listen 8080;
    server_name localhost;

    location / {
        root /var/www/html;
        index index.html;
    }

    location /cgi-bin/ {
        cgi_pass /usr/bin/python3;
        root /var/www/cgi-bin;
    }

    error_page 404 /404.html;
    error_page 403 /403.html;
}
```

Key sections:
- **listen**: Port the server listens on.
- **server_name**: Hostname for virtual hosting.
- **root**: Directory to serve static files.
- **index**: Default file to serve (e.g., `index.html`).
- **cgi_pass**: Path to the CGI interpreter (e.g., PHP, Python).
- **error_page**: Custom error pages for different status codes.

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
├── src/
│   ├── main.cpp            # Entry point of the server
│   ├── server.cpp          # Server logic
│   ├── request.cpp         # HTTP request handling
│   ├── response.cpp        # HTTP response generation
│   ├── cgi.cpp             # CGI handler
│   ├── config_parser.cpp   # Configuration file parser
│   └── worker.cpp          # Handles incoming requests
│
├── include/
│   ├── server.hpp          # Server header
│   ├── request.hpp         # Request class header
│   ├── response.hpp        # Response class header
│   └── cgi.hpp             # CGI class header
│
├── config/
│   ├── default.conf        # Example configuration file
│
├── Makefile                # Build instructions
└── README.md               # This README file
```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
```
