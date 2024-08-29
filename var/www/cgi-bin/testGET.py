#!/usr/bin/env python3
import os
import sys

def handle_get():
    # Retrieve the query string from the environment
    method = os.environ.get("METHOD",)

    # Prepare the response body
    response_body = f"""
				<html>
				<head><title>CGI GET Response</title></head>
				<body>
				<h1>Hello, World!</h1>
				<p>This is a response from a CGI script for a {method} request.</p>
				"""
    for key, values in os.environ.items():
        response_body += f"<p>{key}: {values}</p>\n"
    response_body += """
                </body>
                </html>
                """

    # Calculate the content length of the response body
    response_content_length = len(response_body)

    # Set up headers for the response
    print(f"Status: 200 OK\r\nContent-Type: text/html\r\nContent-Length: {response_content_length}\r\n\r\n")

    # Output the response body
    print(response_body)

def main():
    handle_get()

if __name__ == "__main__":
    main()
