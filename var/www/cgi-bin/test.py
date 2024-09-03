#!/usr/bin/env python3

import os

def handle_get():
    method = os.environ.get("METHOD")
    path_info = os.environ.get("PATH_INFO")
    script_name = os.environ.get("SCRIPT_NAME")

    response_body = f"""
<html>
<head><title>CGI GET Response</title></head>
<body>
<h1>Hello, World!</h1>
<p>This is a response from a CGI script for a {method} request.</p>
<p>Path of the script is: {path_info}</p>
<p>Script name is: {script_name}</p>
<p>Received {method} data: </p>
</body>
</html>
"""
    content_length = len(response_body)
    print(f"Status: 200 OK\r\nContent-Type: text/html\r\nContent-Length: {content_length}\r\n\r\n")
    print(response_body)


if __name__ == "__main__":
    handle_get()
