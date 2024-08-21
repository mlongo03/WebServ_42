#!/usr/bin/env python3

import os
import sys

def handle_get():
    response_body = """
<html>
<head><title>CGI GET Response</title></head>
<body>
<h1>Hello, World!</h1>
<p>This is a response from a CGI script for a GET request.</p>
</body>
</html>
"""
    content_length = len(response_body)
    print(f"Status: 200 OK\r\nContent-Type: text/html\r\nContent-Length: {content_length}\r\n\r\n")
    print(response_body)


def handle_post():
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    post_data = sys.stdin.read(content_length)
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
    content_length = len(response_body)
    print(f"Status: 200 OK\r\nContent-Type: text/html\r\nContent-Length: {content_length}\r\n\r\n")
    print(response_body)


if __name__ == "__main__":
    method = os.environ.get('REQUEST_METHOD', 'GET')
    if method == 'POST':
        handle_post()
    else:
        handle_get()

# import os
# import sys

# def handle_get():
#     print("Status: 200 ok\r\nContent-Type: text/plain\r\n\r\n")
#     print()
#     print("<html>")
#     print("<head><title>CGI GET Response</title></head>")
#     print("<body>")
#     print("<h1>Hello, World!</h1>")
#     print("<p>This is a response from a CGI script for a GET request.</p>")
#     print("</body>")
#     print("</html>")


# def handle_post():
#     content_length = int(os.environ.get('CONTENT_LENGTH', 0))
#     post_data = sys.stdin.read(content_length)
#     print("Status: 200 OK\r\nContent-Type: text/plain\r\n\r\n")
#     print()
#     print("<html>")
#     print("<head><title>CGI POST Response</title></head>")
#     print("<body>")
#     print("<h1>Hello, World!</h1>")
#     print("<p>This is a response from a CGI script for a POST request.</p>")
#     print("<p>Received POST data: {}</p>".format(post_data))
#     print("</body>")
#     print("</html>")

# if __name__ == "__main__":
#     method = os.environ.get('REQUEST_METHOD', 'GET')
#     if method == 'POST':
#         handle_post()
#     else:
#         handle_get()

# import os
# import urllib.parse

# def main():
#     # Set up headers for the response
#     print("Status: 200 OK\r\nContent-Type: text/plain\r\n\r\n")

#     # Retrieve the query string from the environment
#     query_string = os.environ.get("QUERY_STRING", "")

#     # Parse the query string
#     params = urllib.parse.parse_qs(query_string)

#     # Output the received data
#     print(f"Received GET data: {query_string}")

#     # Display the parsed parameters
#     for key, values in params.items():
#         print(f"{key}: {', '.join(values)}")

# if __name__ == "__main__":
#     main()

# #!/usr/bin/env python3

# import os
# import urllib.parse

# def main():
#     # Set up headers for the response
#     print("Status: 200 OK\r\nContent-Type: text/plain\r\n\r\n")

# 	# Retrieve the query string from the environment
#     query_string = os.environ.get("QUERY_STRING", "")

#     # Parse the query string
#     params = urllib.parse.parse_qs(query_string)

#     # Output the received data
#     print(f"Received GET data: {query_string}")
#     # print(f"Environment variables:", os.environ, file=sys.stderr)


#     # Display the parsed parameters
#     for key, values in params.items():
#         print(f"{key}: {', '.join(values)}")

# if __name__ == "__main__":
#     main()


# #!/usr/bin/env python3

# import os
# import urllib.parse
# import sys

# def handle_get_request():
#     # Retrieve the query string from the environment
#     query_string = os.environ.get("QUERY_STRING", "")

#     # Parse the query string
#     params = urllib.parse.parse_qs(query_string)

#     # Output the received data
#     print(f"Received GET data: {query_string}")

#     print(f"Environment variables:", os.environ, file=sys.stderr)


#     # Display the parsed parameters
#     for key, values in params.items():
#         print(f"{key}: {', '.join(values)}")

# def handle_post_request():
#     # Get the content length from the environment
#     content_length = os.environ.get("CONTENT_LENGTH", 0)
#     print(f"Content-Length: {content_length}", file=sys.stderr)

#     try:
#         content_length = int(content_length)
#     except ValueError:
#         print("Invalid content length", file=sys.stderr)
#         content_length = 0

#     # Read the POST data from stdin
#     post_data = sys.stdin.read(content_length)
#     print(f"Received POST data: {post_data}", file=sys.stderr)
#  #!/usr/bin/env python3

# import os
# import urllib.parse
# import sys

# def handle_get_request():
#     # Retrieve the query string from the environment
#     query_string = os.environ.get("QUERY_STRING", "")

#     # Parse the query string
#     params = urllib.parse.parse_qs(query_string)

#     # Output the received data
#     print(f"Received GET data: {query_string}")

#     print(f"Environment variables:", os.environ, file=sys.stderr)


#     # Display the parsed parameters
#     for key, values in params.items():
#         print(f"{key}: {', '.join(values)}")

# def handle_post_request():
#     # Get the content length from the environment
#     content_length = os.environ.get("CONTENT_LENGTH", 0)
#     print(f"Content-Length: {content_length}", file=sys.stderr)

#     try:
#         content_length = int(content_length)
#     except ValueError:
#         print("Invalid content length", file=sys.stderr)
#         content_length = 0

#     # Read the POST data from stdin
#     post_data = sys.stdin.read(content_length)
#     print(f"Received POST data: {post_data}", file=sys.stderr)
#     print(f"Environment variables:", os.environ, file=sys.stderr)

#     # Output the received data
#     print(f"Received POST data: {post_data}")

# def main():
#     print("Content-Type: text/plain\n")

#     method = os.environ.get("REQUEST_METHOD", "GET")

#     if method == "GET":
#         handle_get_request()
#     elif method == "POST":
#         handle_post_request()

# if __name__ == "__main__":
#     main()


#     # Output the received data
#     print(f"Received POST data: {post_data}")

# def main():
#     print("Content-Type: text/plain\n")

#     method = os.environ.get("REQUEST_METHOD", "GET")

#     if method == "GET":
#         handle_get_request()
#     elif method == "POST":
#         handle_post_request()

# if __name__ == "__main__":
#     main()
