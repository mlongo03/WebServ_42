#!/usr/bin/env python3
import sys
import os

if os.environ['REQUEST_METHOD'] == 'POST':
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    post_data = sys.stdin.read(content_length)
    print("Content-type: text/plain\n")
    print(f"Received POST data:\n{post_data}")
else:
    print("Content-type: text/plain\n")
    print("This is a GET request.")
