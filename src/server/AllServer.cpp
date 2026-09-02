/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/02 19:11:06 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AllServer.hpp"
#include "EpollHandler.hpp"
#include "MagicValues.hpp"
#include "SocketFd.hpp"

#include <algorithm>
#include <functional>
#include <sys/epoll.h>
#include <cstring>
#include <array>
#include <sys/socket.h>

AllServers::AllServers(MainConfig &main_config) : epoll_fd_(init_epoll())
{
	for (ServerConfig &other_server : main_config.http_conf.servers_confs)
	{
		Server new_server = Server(other_server);
		new_server.add_sockets(all_fds_, epoll_fd_);
		all_servers_.push_back(std::move(new_server));
	}
	for (std::unique_ptr<EpollHandler> &connection_fd : all_fds_)
	{
		SocketFd &socket_fd = dynamic_cast<SocketFd &>(*connection_fd);
		socket_fd.listen();
		socket_fd.add_to_epoll();
	}
}

// epoll_create(1) 1 = start size but Nowadays its dynamicly allocated, so 1 is
// enough
int AllServers::init_epoll()
{
	int epoll_fd = -1;
	if (epoll_fd == -1)
	{
		epoll_fd = epoll_create(1);
		if (epoll_fd < 0)
			throw std::runtime_error(std::string("epoll_create: ") +
									 std::strerror(errno));
	}
	return epoll_fd;
}

// add to all_fds_
void AllServers::add_handel(std::unique_ptr<EpollHandler> &&new_handel)
{
	all_fds_.push_back(std::move(new_handel));
}

Server &AllServers::at(size_t index)
{
	return all_servers_.at(index);
}

void AllServers::wait_epoll(void)
{
	static std::array<struct epoll_event, MAX_EVENTS> events_buffer_array;

	int events_count = ::epoll_wait(epoll_fd_, events_buffer_array.data(),
									MAX_EVENTS, EPOLL_TIMEOUT_MS);
	if (events_count == -1)
	{
		throw std::runtime_error(std::string("epoll_wait: ") +
								 std::strerror(errno));
	}

	for (int i = 0; i < events_count; ++i)
	{
		static_cast<EpollHandler *>(events_buffer_array[i].data.ptr)
			->on_epoll_event(*this);
	}
}

void AllServers::cleanup_all_fds(void)
{
	all_fds_.erase(std::remove_if(all_fds_.begin(), all_fds_.end(),
								  std::mem_fn(&EpollHandler::is_remove_me)),
				   all_fds_.end());
}

// const CloseFd &AllServers::get_epoll_fd(void) const
// {
// 	return epoll_fd_;
// }
