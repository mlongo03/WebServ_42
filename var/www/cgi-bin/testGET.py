#!/usr/bin/env python3

import os

print("Content-Type: text/html\n")
print("<html><body>")
print("<h1>Hello from CGI!</h1>")
print(f"<p>Request Method: {os.environ.get('REQUEST_METHOD')}</p>")
print(f"<p>Path Info: {os.environ.get('PATH_INFO')}</p>")
print("</body></html>")


query_string = os.environ.get('QUERY_STRING')
print("Content-Type: text/plain\n")
print("Query String Data: ", query_string)
