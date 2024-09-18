#!/usr/bin/env python3
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import os
import cgi


UPLOAD_DIR = "/home/ale/Desktop/WebServ_42/var/www/"  # Directory where uploaded files will be saved

def handle_post():
    # Ensure the upload directory exists
    if not os.path.exists(UPLOAD_DIR):
        os.makedirs(UPLOAD_DIR)

    # Get the form data
    form = cgi.FieldStorage()

    # Check if the file field is in the form
    if "file" in form:
        file_item = form["file"]

        # If a file was uploaded
        if file_item.filename:
            # Sanitize the file name to prevent directory traversal attacks
            filename = os.path.basename(file_item.filename)

            # Save the uploaded file to the upload directory
            file_path = os.path.join(UPLOAD_DIR, filename)
            with open(file_path, "wb") as f:
                f.write(file_item.file.read())

            # Prepare a success response
            response_body = f"""
            <html>
            <head><title>Upload Successful</title></head>
            <body>
            <h1>File Upload Successful</h1>
            <p>File uploaded as: {filename}</p>
            </body>
            </html>
            """
        else:
            # No file was uploaded
            response_body = """
            <html>
            <head><title>Upload Failed</title></head>
            <body>
            <h1>Upload Failed</h1>
            <p>No file was uploaded.</p>
            </body>
            </html>
            """
    else:
        # No file field in form
        response_body = """
        <html>
        <head><title>Upload Failed</title></head>
        <body>
        <h1>Upload Failed</h1>
        <p>Form did not contain a file field.</p>
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
    method = os.environ.get('REQUEST_METHOD', 'GET')
    if method == 'POST':
        handle_post()
    else:
        # If the request method is not POST, return a 405 Method Not Allowed response
        response_body = """
        <html>
        <head><title>405 Method Not Allowed</title></head>
        <body>
        <h1>405 Method Not Allowed</h1>
        <p>This server only handles POST requests.</p>
        </body>
        </html>
        """
        response_content_length = len(response_body)
        print(f"Status: 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: {response_content_length}\r\n\r\n")
        print(response_body)

if __name__ == "__main__":
    main()
