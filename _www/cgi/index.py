import os
import sys

length = int(os.environ.get("CONTENT_LENGTH", "0"))
body = sys.stdin.read(length)

response = (
    "Content-Type: text/plain\r\n"
    "\r\n"
    "method=" + os.environ.get("REQUEST_METHOD", "") + "\n"
    "query=" + os.environ.get("QUERY_STRING", "") + "\n"
    "body=" + body + "\n"
)

sys.stdout.write(response)