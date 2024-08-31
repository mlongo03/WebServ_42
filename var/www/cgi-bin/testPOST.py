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
    response_body = """
    <html>
    <head><title>CGI POST Response</title></head>
    <body>
    <h1>Hello, World!</h1>
    <p>This is a response from a CGI script for a POST request.</p>
    <p>Received POST data:</p>
    <ul>
    """
    for key, values in params.items():
        response_body += f"<li>{key}: {', '.join(values)}</li>\n"

    response_body += """
    </ul>
    </body>
    </html>
    """

    # Calculate the content length of the response body
    response_content_length = len(response_body)

    # Set up headers for the response
    print(f"Status: 200 OK\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: {response_content_length}\r\n\r\n")

    # Output the response body
    print(response_body)

if __name__ == "__main__":
    handle_post()
