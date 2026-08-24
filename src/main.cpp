/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:58:02 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/24 17:21:58 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Server.hpp"
#include "logging.hpp"
#include "normalFucntions.hpp"

#include <sys/socket.h>

// char response2[] =
// 	"HTTP/1.1 200 OK\r\n"
// 	"Content-Type: text/html; charset=UTF-8\r\n\r\n"
// 	"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
// 	"<style>body { background-color: #111 }"
// 	"h1 { font-size:4cm; text-align: center; color: black;"
// 	" text-shadow: 0 0 2mm red}</style></head>"
// 	"<body><h1>Goodbye, world!</h1></body></html>\r\n\r\n";

int main(int argc, const char *argv[])
{
	try
	{
		MainConfig main_config;
		const std::filesystem::path config_file = check_input_args(argc, argv);
		{
			Config config{config_file, main_config};
			LOG(LOG_DEBUG, config.to_string());
		}

		AllServers all_servers(main_config);

		all_servers.listen();
		all_servers.add_to_epoll();
		all_servers.wait_epoll();

		// while (true)
		// {
		// 	std::vector<int> new_fds = all_servers.accept();
		// 	for (int fd : new_fds)
		// 	{
		// 		send(fd, response2, sizeof(response2) - 1, 0);
		// 	}
		// }

		// TODO: make good webserver loop and system calls etc.
		//  first_website_copy_of_internet();
	}
	catch (const std::exception &e)
	{
		LOG(LOG_ERROR, e.what());
		return EXIT_FAILURE;
	}
	catch (...)
	{
		LOG(LOG_ERROR, "Unknown exception occurred that is not derived from "
					   "std::exception.");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
