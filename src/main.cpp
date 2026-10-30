/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:58:02 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 13:07:50 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AllServer.hpp"
#include "logging.hpp"
#include "Signals.hpp"

#include <sys/socket.h>

int main(int argc, const char *argv[])
{
	try
	{
		init_signal_handling();
		AllServers all_servers = MainConfigParser(argc, argv);

		while (g_stop_server.load() == false)
		{
			all_servers.wait_epoll();
			all_servers.check_timeouts();
			all_servers.cleanup_all_fds();
			LOG(LOG_DEBUG, "tick");
		}
	}
	catch (const std::exception &e)
	{
		LOG(LOG_ERROR, e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
