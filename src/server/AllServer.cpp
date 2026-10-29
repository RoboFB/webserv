/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 14:02:06 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AllServer.hpp"
#include "EpollHandler.hpp"
#include "MagicValues.hpp"
#include "logging.hpp"

#include <algorithm>
#include <functional>
#include <sys/epoll.h>
#include <cstring>
#include <array>
#include <sys/socket.h>

AllServers::AllServers(MainConfigParser &&main_config) : epoll_fd_(init_epoll())
{
	// SocketFd and Connection retain a pointer to their owning Server.  Create
	// every Server in its final vector storage before creating those handlers:
	// using a temporary Server here would leave the handlers with a dangling
	// pointer once that temporary is moved/destroyed.
	all_servers_.reserve(main_config.http_conf.servers_confs.size());
	for (ServerConfigParser &other_server : main_config.http_conf.servers_confs)
	{
		all_servers_.emplace_back(other_server);
	}
	for (const Server &server : all_servers_)
	{
		server.add_sockets(all_fds_, epoll_fd_);
	}
}

AllServers::~AllServers()
{
	stop_all();
}

// epoll_create(1) 1 = start size but Nowadays its dynamicly allocated, so 1 is
// enough
int AllServers::init_epoll() const
{
	int epoll_fd = epoll_create(1);
	if (epoll_fd < 0)
		throw std::runtime_error(std::string("epoll_create: ") +
								 std::strerror(errno));
	return epoll_fd;
}

// add to all_fds_
void AllServers::add_handler(std::unique_ptr<EpollHandler> &&new_handler)
{
	all_fds_.push_back(std::move(new_handler));
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
		LOG(LOG_DEBUG, "epoll_wait: " + std::string(std::strerror(errno)));
	}

	for (int i = 0; i < events_count; ++i)
	{
		EpollHandler *p_handel =
			static_cast<EpollHandler *>(events_buffer_array[i].data.ptr);
		if (p_handel == nullptr)
		{
			LOG(LOG_ERROR, "epoll_wait: data.ptr is nullptr");
			continue;
		}

		const uint32_t &event = events_buffer_array[i].events;
		if (event & EPOLL_EVENTS::EPOLLERR || event & EPOLL_EVENTS::EPOLLHUP)
		{
			LOG(LOG_ERROR, "epoll_wait: EPOLLERR or EPOLLHUP on fd ");
			p_handel->stop_listening();
			continue;
		}
		p_handel->on_epoll_event(*this, events_buffer_array[i].events);
	}
}

void AllServers::check_timeouts(void)
{
	for (std::unique_ptr<EpollHandler> &connection_fd : all_fds_)
	{
		if (connection_fd->is_timeout())
			connection_fd->stop_listening();
	}
}

void AllServers::stop_all(void)
{
	for (std::unique_ptr<EpollHandler> &connection_fd : all_fds_)
	{
		connection_fd->stop_listening();
	}
	all_fds_.clear();
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
