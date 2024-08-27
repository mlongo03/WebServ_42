#!/usr/bin/env python3
import os
import sys
import urllib.parse

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
    # Check if the request method is GET
    method = os.environ.get('REQUEST_METHOD', 'GET')
    if method == 'GET':
        handle_get()
    else:
        # Set up headers for the response
        error_message = "405 Method Not Allowed"
        print(f"Status: 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: {len(error_message)}\r\n\r\n")
        # Output the error message
        print(error_message)
        
if __name__ == "__main__":
    main()
