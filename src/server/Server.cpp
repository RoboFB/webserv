/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/21 02:09:23 by modiepge         ###   ########.fr       */
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
}

void Server::add_sockets(std::vector<std::unique_ptr<EpollHandler>> &all_fds,
						 const CloseFd &epoll_fd) const
{
	for (const struct addrinfo *current_addr = addr_listen_list_.get();
		 current_addr != nullptr; current_addr = current_addr->ai_next)
	{
		all_fds.push_back(
			std::make_unique<SocketFd>(current_addr, this, epoll_fd));
	}
}

static bool is_location_match(const std::string& request_path, const std::string& location_path) {
	if (location_path == "/")
		return (!request_path.empty() && request_path[0] == '/');
	if (request_path.compare(0, location_path.size(), location_path) != 0)
		return (false);
	return (request_path.size() == location_path.size() || request_path[location_path.size()] == '/');
}

const Location* Server::find_location(const std::string& request_path) const {
	const Location* best_match = nullptr;
	for (const Location& location : locations_) {
		if (!is_location_match(request_path, location.location_path))
			continue;
		if (best_match == nullptr || location.location_path.size() > best_match->location_path.size())
			best_match = &location;
	}
	return (best_match);
}