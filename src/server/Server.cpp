/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 19:29:59 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include "SocketFd.hpp"

Server::Server(ServerConfig &server_config, std::vector<SocketFd> &all_fds)
	: locations_(), addr_listen_list_(std::move(server_config.listen))
{
	for (LocationConfig &location_config : server_config.locations_confs)
	{
		locations_.push_back(Location(location_config));
	}

	for (const struct addrinfo *current_addr = addr_listen_list_.get();
		 current_addr != nullptr; current_addr = current_addr->ai_next)
	{
		all_fds.push_back(SocketFd(current_addr));
	}
}
