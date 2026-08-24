/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/24 17:57:52 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "MagicValues.hpp"

#include <iostream>
#include <sys/epoll.h>
#include <cstring>

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

void AllServers::listen(void)
{
	for (Server &server : all_servers_)
	{
		server.listen();
	}
}

std::vector<int> AllServers::accept(void)
{
	std::vector<int> new_fds;
	for (Server &server : all_servers_)
	{
		for (auto fd : server.accept())
		{
			new_fds.push_back(fd);
		}
	}
	return new_fds;
}

Server &AllServers::at(size_t index)
{
	return all_servers_.at(index);
}

Server::Server(ServerConfig &server_config)
	: locations_(), addr_listen_list_(std::move(server_config.listen)),
	  socket_fds_(addr_listen_list_)
{
	for (LocationConfig &location_config : server_config.locations_confs)
	{
		locations_.push_back(Location(location_config));
	}
}

void Server::add_to_epoll(const CloseFd &epoll_fd)
{
	socket_fds_.add_to_epoll(epoll_fd, this);
}

void Server::listen(void)
{
	socket_fds_.listen();
}

std::vector<int> Server::accept(void)
{
	return socket_fds_.accept();
}

char response2[] =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n\r\n"
	"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
	"<style>body { background-color: #111 }"
	"h1 { font-size:4cm; text-align: center; color: black;"
	" text-shadow: 0 0 2mm red}</style></head>"
	"<body><h1>Goodbye, world!</h1></body></html>\r\n\r\n";

void AllServers::wait_epoll(void)
{
	struct epoll_event events_buffer_array[MAX_EVENTS];

	while (true)
	{
		int events_count =
			epoll_wait(epoll_fd_, events_buffer_array, MAX_EVENTS, -1);
		if (events_count == -1)
		{
			throw std::runtime_error(std::string("epoll_wait: ") +
									 std::strerror(errno));
		}

		for (int i = 0; i < events_count; ++i)
		{
			Server *server =
				static_cast<Server *>(events_buffer_array[i].data.ptr);
			std::cout << "server: " << (void *)server << std::endl;
			std::vector<int> new_fds = server->accept();
			for (int fd : new_fds)
			{
				send(fd, response2, sizeof(response2) - 1, 0);
			}
			// server->add_to_epoll(epoll_fd_);

			// else
			// {
			// 	do_use_fd(events_buffer_array[i].data.fd);
			// }
		}
	}
}