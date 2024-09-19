#!/usr/bin/env python3

import os

def main():
    print(f"Status: 200 OK\r\nContent-Type: text/html\r\n\r\n")
    print()  # Blank line required after headers

    # Try to access a file using a relative path
    try:
        with open("./files/data.txt", "r") as file:
            file_content = file.read()
            print("<html><body>")
            print("<h1>CGI Script Test</h1>")
            print("<p>Current Directory: {}</p>".format(os.getcwd()))
            print("<p>Content of ./files/data.txt:</p>")
            print("<pre>{}</pre>".format(file_content))
            print("</body></html>")
    except IOError as e:
        print("<html><body>")
        print("<h1>Error</h1>")
        print("<p>Could not open file './files/data.txt'.</p>")
        print("<p>Error: {}</p>".format(str(e)))
        print("<p>Current Directory: {}</p>".format(os.getcwd()))
        print("</body></html>")

if __name__ == "__main__":
    main()
