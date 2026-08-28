/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:58:02 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 19:28:54 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "AllServer.hpp"
#include "logging.hpp"
#include "normalFucntions.hpp"

#include <sys/socket.h>

int main(int argc, const char *argv[])
{
	try
	{
		MainConfig main_config;
		const std::filesystem::path config_file = check_input_args(argc, argv);
		{
			Config config{config_file, main_config};
		}

		AllServers all_servers(main_config);

		while (true)
		{
			all_servers.wait_epoll();
		}
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
