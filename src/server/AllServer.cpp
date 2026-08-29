/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/29 11:11:58 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AllServer.hpp"
#include "EpollHandler.hpp"
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
		// data.ptr is either a SocketFd* (listener) or a Connection* (client);
		// both derive from EpollHandler, so the vtable picks the right code
		// path directly, no linear search over listen_fds_/accepted_fds_.
		EpollHandler *handler =
			static_cast<EpollHandler *>(events_buffer_array.at(i).data.ptr);
		handler->on_epoll_event(*this);
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

void AllServers::accept_connection(SocketFd &listener)
{
	int new_fd = listener.accept();
	if (new_fd < 0)
		return; // error already logged in SocketFd::accept

	auto insert_result = accepted_fds_.emplace(
		new_fd, Connection(listener.server(), CloseFd(std::move(new_fd))));
	Connection &connection = insert_result.first->second;

	connection.add_to_epoll(epoll_fd_);
	connection.send(response2); // todo: for testing only, remove later
}

void AllServers::close_connection(int fd)
{
	accepted_fds_.erase(fd);
}
