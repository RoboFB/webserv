/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 18:10:42 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AllServer.hpp"
#include "MagicValues.hpp"
#include "logging.hpp"

// #include <algorithm>
#include <sys/epoll.h>
#include <cstring>
#include <array>

AllServers::AllServers(MainConfig &main_config) : epoll_fd_(-1)
{
	for (ServerConfig &other_server : main_config.http_conf.servers_confs)
	{
		all_servers_.push_back(Server(other_server));
	}
}

// epoll_create(1) 1 = start size but Nowadays its dynamicly allocated, so 1 is
// enough
void AllServers::add_to_epoll(void)
{
	if (epoll_fd_ == -1)
	{
		epoll_fd_ = epoll_create(1);
		if (epoll_fd_ < 0)
			throw std::runtime_error(std::string("epoll_create: ") +
									 std::strerror(errno));
	}
	for (Server &server : all_servers_)
	{
		server.add_to_epoll(epoll_fd_);
	}
}

void AllServers::listen(void) const
{
	for (const Server &server : all_servers_)
	{
		server.listen();
	}
}

std::vector<CloseFd> AllServers::accept(void) const
{
	std::vector<CloseFd> new_fds;
	for (const Server &server : all_servers_)
	{
		for (auto &&fd : server.accept())
		{
			new_fds.push_back(std::move(fd));
		}
	}
	return new_fds;
}

Server &AllServers::at(size_t index)
{
	return all_servers_.at(index);
}

void AllServers::wait_epoll(void)
{
	static std::array<struct epoll_event, MAX_EVENTS> events_buffer_array;

	int events_count = epoll_wait(epoll_fd_, events_buffer_array.data(),
								  MAX_EVENTS, EPOLL_TIMEOUT);
	if (events_count == -1)
	{
		throw std::runtime_error(std::string("epoll_wait: ") +
								 std::strerror(errno));
	}

	for (int i = 0; i < events_count; ++i)
	{
		int socket_fd = events_buffer_array.at(i).data.fd;
		LOG(LOG_DEBUG, "epoll_wait: socket_fd: " + std::to_string(socket_fd));

		for (Server &server : all_servers_)
		{
			if (server.smart_accept(socket_fd))
			{
				break;
			}
		}
	}
}
