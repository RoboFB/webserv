/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 13:39:09 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/30 15:04:08 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "testing.hpp"
#include "config.hpp"
#include "logging.hpp"
#include <err.h>
#include <filesystem>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>



void test_logging(void)
{
	LOG(LOG_DEBUG, "Server is initializing...");
	LOG(LOG_INFO, "Starting the web server...");
	LOG(LOG_WARN, "This is a warning message.");
	LOG(LOG_ERROR, "An error occurred while starting the server.");
}

void test_lexer(void)
{
	std::filesystem::path config_path = "./config/hard_tests.conf";
	main_parsing(config_path);
}




char response[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
"<style>body { background-color: #111 }"
"h1 { font-size:4cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1>Goodbye, world!</h1></body></html>\r\n";

void first_website_copy_of_internet(void)
{
	std::cout << "HELLO \n";
	std::cout << response << std::endl;

	int one = 1, client_fd;
	struct sockaddr_in svr_addr, cli_addr;
	socklen_t sin_len = sizeof(cli_addr);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		err(1, "can't open socket");

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

	int port = 8080;
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = INADDR_ANY;
	svr_addr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1) {
		close(sock);
		err(1, "Can't bind");
	}

	listen(sock, 5);
	while (1) {
		client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
		printf("got connection\n");

		if (client_fd == -1) {
			perror("Can't accept");
			continue;
		}

		write(client_fd, response, sizeof(response) - 1); /*-1:'\0'*/
		close(client_fd);
	}
}



void first_website(void)
{
	// const char *name = "google.com";
	// const char *service = "https";
	// const struct addrinfo *req = NULL;
	// struct addrinfo **pai = NULL;


	// getaddrinfo(name, service, req, pai);
	// std::cout << "Host: " << pai << std::endl;

	// std::cout << "Address info for " << name << ":" << std::endl;
	// for (int i = 0; pai[i] != NULL; i++) {
	// 	std::cout << "Host: " << pai[i] << std::endl;
	// }




	int status;
	struct addrinfo hints{};
	hints.ai_flags = AI_PASSIVE;		// fill in my IP for me
	hints.ai_family = AF_UNSPEC;		// don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets
	struct addrinfo *servinfo = NULL;		// will point to the results


	std::cout << servinfo << std::endl;
	if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
		fprintf(stderr, "gai error: %s\n", gai_strerror(status));
		exit(1);
	}


	std::cout << servinfo << std::endl;



	/* 
	
	TODO:
	check out why the output is wirded with servinfo and after that work with benj guid make basic address working

	*/


	// servinfo now points to a linked list of 1 or more
	// struct addrinfos

	// ... do everything until you don't need servinfo anymore ....

	freeaddrinfo(servinfo); // free the linked-list

}