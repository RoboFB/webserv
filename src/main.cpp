/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:58:02 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/12 19:16:37 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "logging.hpp"
#include "testing.hpp"
#include <unistd.h>

char response2[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
"<style>body { background-color: #111 }"
"h1 { font-size:4cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1>Goodbye, world!</h1></body></html>\r\n\r\n";


int main(int argc, const char *argv[])
{
	// first_website_copy_of_internet();
	// first_website();
	try
	{
		const std::filesystem::path config_file = check_input_args(argc, argv);
		Config config(config_file);
		
		auto res = config.get_server_context(0).listen.get();
		int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

		int x = bind(sockfd, res->ai_addr, res->ai_addrlen);
		(void)x; // suppress unused variable warning

		int y = listen(sockfd, 10); 
		(void)y; // suppress unused variable warning


		struct sockaddr_storage their_addr;
		socklen_t addr_size = sizeof(their_addr);

		while (true)
		{

			int new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
			&addr_size);
			
			send(new_fd, response2, sizeof(response2) - 1, 0);
		}
			


		//TODO: make good webserver loop and system calls etc. 
		// first_website_copy_of_internet();
	}
	catch (const std::exception& e) 
	{
		LOG(LOG_ERROR, e.what());
		return EXIT_FAILURE;
	}
	catch (...) 
	{
		LOG(LOG_ERROR, "Unknown exception occurred that is not derived from std::exception.");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

