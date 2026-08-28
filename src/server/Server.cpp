/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 18:18:42 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <algorithm>

Server::Server(ServerConfig &server_config)
	: locations_(), addr_listen_list_(std::move(server_config.listen)),
	  socket_fds_()
{
	for (LocationConfig &location_config : server_config.locations_confs)
	{
		locations_.push_back(Location(location_config));
	}

	for (const struct addrinfo *current_addr = addr_listen_list_.get();
		 current_addr != nullptr; current_addr = current_addr->ai_next)
	{
		socket_fds_.insert(SocketFd(current_addr));
	}
}

void Server::add_to_epoll(const CloseFd &epoll_fd) const
{
	for (const SocketFd &fd : socket_fds_)
	{
		fd.add_to_epoll(epoll_fd);
	}
}
void Server::listen(void) const
{
	for (const SocketFd &fd : socket_fds_)
	{
		fd.listen();
	}
}

std::vector<CloseFd> Server::accept() const
{
	std::vector<CloseFd> new_fds;
	for (const SocketFd &socket_fd : socket_fds_)
	{
		new_fds.push_back(socket_fd.accept());
	}
	return new_fds;
}

char response2[] =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n\r\n"
	"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
	"<style>body { background-color: #111 }"
	"h1 { font-size:4cm; text-align: center; color: black;"
	" text-shadow: 0 0 2mm red}</style></head>"
	"<body><h1>Goodbye, world!</h1></body></html>\r\n\r\n";

// returns true if accept
bool Server::smart_accept(const int compare_with_me)
{
	std::set<SocketFd>::const_iterator found =
		std::find_if(socket_fds_.cbegin(), socket_fds_.cend(),
					 [compare_with_me](const SocketFd &fd)
					 { return fd == compare_with_me; });
	if (found != socket_fds_.cend())
	{
		accepted_fds_.push_back(found->accept());
		add_to_epoll(accepted_fds_.back());
		send(accepted_fds_.back(), response2, sizeof(response2) - 1, 0);

		return true;
	}
	return false;
}
