/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 20:31:27 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AllServer.hpp"
#include "MagicValues.hpp"
#include "logging.hpp"

#include <sys/epoll.h>
#include <cstring>
#include <array>
#include <sys/socket.h>

AllServers::AllServers(MainConfig &main_config) : epoll_fd_(-1)
{
	for (ServerConfig &other_server : main_config.http_conf.servers_confs)
	{
		all_servers_.push_back(Server(other_server, listen_fds_));
	}
	init_epoll();
	for (const SocketFd &connection_fd : listen_fds_)
	{
		connection_fd.listen();
		connection_fd.add_to_epoll(epoll_fd_);
	}
}

// epoll_create(1) 1 = start size but Nowadays its dynamicly allocated, so 1 is
// enough
void AllServers::init_epoll()
{
	if (epoll_fd_ == -1)
	{
		epoll_fd_ = epoll_create(1);
		if (epoll_fd_ < 0)
			throw std::runtime_error(std::string("epoll_create: ") +
									 std::strerror(errno));
	}
	// connection_fd.add_to_epoll(epoll_fd_);
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
		int event_fd = events_buffer_array.at(i).data.fd;
		LOG(LOG_DEBUG, "epoll_wait: event_fd: " + std::to_string(event_fd));

		if (smart_accept(event_fd))
		{
			continue;
		}
		// try_receive(event_fd);
	}
}

std::string response2 =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n\r\n"
	"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
	"<style>body { background-color: #111 }"
	"h1 { font-size:4cm; text-align: center; color: black;"
	" text-shadow: 0 0 2mm red}</style></head>"
	"<body><h1>Goodbye, world!</h1></body></html>\r\n\r\n";

// returns true if accept
bool AllServers::smart_accept(const int compare_with_me)
{
	for (const SocketFd &connection_fd : listen_fds_)
	{
		if (connection_fd == compare_with_me)
		{
			accepted_fds_.push_back(CloseFd(connection_fd.accept()));
			accepted_fds_.back().add_to_epoll(epoll_fd_);
			accepted_fds_.back().send(
				response2); // todo: for testing only, remove later
			return true;
		}
	}
	return false;
}

// bool AllServers::try_receive(const int compare_with_me)
// {
// 	for (std::vector<Connection>::iterator it = accepted_fds_.begin();
// 		 it != accepted_fds_.end(); ++it)
// 	{
// 		if (*it == compare_with_me)
// 		{
// 			if (it->receive() <= 0)
// 			{
// 				accepted_fds_.erase(it);
// 			}
// 			return true;
// 		}
// 	}
// 	return false;
// }
