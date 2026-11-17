_This project has been created as part of the 42 curriculum by rgohrig and modiepge._

# webserv

Webserv is a small http server implementing GET, POST and DELETE

## Description
Webserv is a learning project intended to familiarize students at 42 with the http protocol, server client interactions, CGI, sockets and polling. It is written in C++17.

With a token-based __config-parser__ with inheritance, a single central polling loop, protection against accessing folders outside specified webroots as well as __auto-indexing__, __redirects__, custom __error pages__ and arbitrary extension-based __CGI__ execution given appropriate binaries (e. g. PHP, Python, Go).

Methods `GET`, `POST`, `DELETE` are implemented.

__Cookies__ are implemented to work by default and are controlled by a given application, to pass via headers, enabling sessions as long as the given CGI script uses them.

## Instructions

Minimum requirements: Linux (for `epoll()`), C++17 and Make

To __setup__: use `make` in the root of the project.
```sh
make
```

To __run__:
``` sh
./webserv config/demo.conf
```

For concurrency and load tests:
``` sh
siege -c 128 -t 10S http://localhost:5001
```

## Resources
- [conventional commits](https://www.conventionalcommits.org/en/v1.0.0/)
- [nginx config file keywords Strong!](https://nginx.org/en/docs/dirindex.html)
- [tutorial of some nginx file specs](https://www.digitalocean.com/community/tutorials/understanding-the-nginx-configuration-file-structure-and-configuration-contexts)
- [beej.us guide for networking](https://beej.us/guide/bgnet/html)

### AI Usage
- Information on relevant topics
- Spell checking
- Technical guidance:
	- Discovery of otherwise unnoticed cases and issues.
	- Pointing out inappropriate/habitual C patterning and explaining C++ concepts and functions based on those principles in C terms.
	- Chore work i. e. the mime type function, demo