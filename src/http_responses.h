#pragma once

#define HTML_HEADER "Content-Type: text/html\r\n"\
"\r\n"\
"<!DOCTYPE html>\r\n"\
"<html>\r\n"\
"<body>\r\n"\
"\r\n"

#define HTML_FOOTER "\r\n"\
"</body>\r\n"\
"</html>\r\n"

#define HTML_INVALID_METHOD "HTTP/1.1 501 Not Implemented\r\n"

#define HTML_INVALID_PROTOCOL "HTTP/1.1 505 HTTP Version Not Supported\r\n"

#define HTML_NOT_FOUND "HTTP/1.1 400 Not Found\r\n" \
HTML_HEADER \
"<p>NOT FOUND</p>\r\n" \
HTML_FOOTER

#define HTML_SUCCESS "HTTP/1.1 200 OK\r\n" \
HTML_HEADER \
"<p>Welcome to my static webserver!</p>\r\n" \
HTML_FOOTER

