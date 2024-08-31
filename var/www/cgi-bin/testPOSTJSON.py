#!/usr/bin/env python3
import os
import sys
import json

def handle_post():
    # Retrieve the content length from the environment
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))

    # Read the POST body from stdin
    post_body = sys.stdin.read(content_length)

    # Parse the POST body as JSON
    try:
        params = json.loads(post_body)
    except json.JSONDecodeError:
        params = {"error": "Invalid JSON"}

    # Prepare the response body as a JSON object
    response_body = f"""
		<htlm>
		<head><title>CGI POST with JSON Response</title></head>
		<body>
		<h1>Hello, World!</h1>
		<p>This is a response from a CGI script for a POST request.</p>
		<p>Received POST data:</p>
		<pre>
			{json.dumps(params)}
		</pre>
		</body>
		</html>
		"""

    # Calculate the content length of the response body
    response_content_length = len(response_body)

    # Set up headers for the response
    print(f"Status: 200 OK\r\nContent-Type: application/json\r\nContent-Length: {response_content_length}\r\n\r\n")

    # Output the response body
    print(response_body)

if __name__ == "__main__":
    handle_post()
