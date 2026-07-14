# Server Config file

# Class structure:

ConfigMain 1x
	ConfigHttp (http) 1x
		- error_page ?x
		- client_max_body_size ?x
		- autoindex ?x
		- root ?x
		- index ?x
		ConfigServer (server) ?x
			- listen ?x
			(- server_name ?x)
    		- return ?x
			Config Location (location) ?x
				- limit_except ?x


# Infos by Robin:
- Don't implement every nginx config
- ignore for example: events, other contexts, don't jain locations


# Subject todos:
- [~] Get inspiration from the `server` section of NGINX.
- [~] Define interface:port pairs on which your server will listen (multiple websites served by your program).

- [~] Set up default error pages.
- [~] Set the maximum allowed size for client request bodies.

- [~] Specify URL/route rules or configurations (no regex required), including:
	- [~] List of accepted HTTP methods for the route.
	- [~] HTTP redirection.
	- [~] Directory where the requested file should be located (e.g. if URL `/kapouet` is rooted to `/tmp/www`, URL `/kapouet/pouic/toto/pouet` will search for `/tmp/www/pouic/toto/pouet`).
    - [~] Enable or disable directory listing.
    - [~] Default file to serve when the requested resource is a directory.
    - [ ] Authorize client uploads and provide the storage location.
    - [ ] Execute CGI based on file extension (for example `.php`).
        - [ ] Ensure CGI environment variables include full client request and arguments.
        - [ ] For chunked requests, unchunk before passing to CGI (CGI expects EOF as end of body).
        - [ ] For CGI output without `content_length`, treat EOF as end of returned data.
        - [ ] Run CGI in the correct directory for relative path file access.
        - [ ] Support at least one CGI (php-cgi, Python, etc.).



### todos general
- [ ] Optionally add other config rules (e.g. server name for virtual hosts).
- [ ] Provide configuration files and default files to test and demonstrate every feature during evaluation.

- [ ] Use the provided tester if helpful.
- [ ] Test with more than one program.
- [ ] Write additional tests in a suitable language (Python, Golang, C, or C++).
- [ ] Compare behavior with NGINX when in doubt about specific behavior.
- [ ] Ensure resilience: server remains operational at all times.