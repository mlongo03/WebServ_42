#!/usr/bin/env python3
import os
import sys
import urllib.parse

def handle_post():
    # Retrieve the content length from the environment
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))

    # Read the POST body from stdin
    post_body = sys.stdin.read(content_length)

    # Parse the POST body
    params = urllib.parse.parse_qs(post_body)

    # Prepare the response body
    response_body = f"""
						<html>
						<head><title>CGI POST Response</title></head>
						<body>
						<h1>Hello, World!</h1>
						<p>This is a response from a CGI script for a POST request.</p>
						<p>Received POST data: {post_data}</p>
						</body>
						</html>
					"""
    for key, values in params.items():
        response_body += f"{key}: {', '.join(values)}\n"

    # Calculate the content length of the response body
    response_content_length = len(response_body)

    # Set up headers for the response
    print(f"Status: 200 OK\r\nContent-Type: text/html\r\nContent-Length: {response_content_length}\r\n\r\n")

    # Output the response body
    print(response_body)

def handle_get():
    # Retrieve the query string from the environment
    query_string = os.environ.get("QUERY_STRING", "")

    # Parse the query string
    params = urllib.parse.parse_qs(query_string)

    # Prepare the response body
    response_body = f"Received GET data: {query_string}\n"
    for key, values in params.items():
        response_body += f"{key}: {', '.join(values)}\n"

    # Calculate the content length of the response body
    response_content_length = len(response_body)

    # Set up headers for the response
    print(f"Status: 200 OK\r\nContent-Type: text/html\r\nContent-Length: {response_content_length}\r\n\r\n")

    # Output the response body
    print(response_body)

def main():
    method = os.environ.get('REQUEST_METHOD', 'GET')
    if method == 'POST':
        handle_post()
    else:
        handle_get()

if __name__ == "__main__":
    main()
