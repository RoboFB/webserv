/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/31 18:32:31 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include "SocketFd.hpp"

Server::Server(ServerConfig &server_config)
	: locations_(), addr_listen_list_(std::move(server_config.listen))
{
	for (LocationConfig &location_config : server_config.locations_confs)
	{
		locations_.push_back(Location(location_config));
	}

	// for (const struct addrinfo *current_addr = addr_listen_list_.get();
	// 	 current_addr != nullptr; current_addr = current_addr->ai_next)
	// {
	// 	all_fds.push_back(SocketFd(current_addr, this));
	// }
}

void Server::add_sockets(
	std::vector<std::unique_ptr<EpollHandler>> &all_fds) const
{
	for (const struct addrinfo *current_addr = addr_listen_list_.get();
		 current_addr != nullptr; current_addr = current_addr->ai_next)
	{
		all_fds.push_back(std::make_unique<SocketFd>(current_addr, this));
	}
}
