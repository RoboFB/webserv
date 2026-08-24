/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/20 21:39:15 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

AllServers::AllServers(MainConfig &main_config)
{
	for (ServerConfig &other_server : main_config.http_conf.servers_confs)
	{
		all_servers.push_back(Server(other_server));
	}
}

void AllServers::listen()
{
	for (Server &server : all_servers)
	{
		server.listen();
	}
}

std::vector<int> AllServers::accept(void)
{
	std::vector<int> new_fds;
	for (Server &server : all_servers)
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
	return all_servers.at(index);
}


Server::Server(ServerConfig &server_config) : locations(), addr_listen_list(std::move(server_config.listen)), socket_fds(addr_listen_list)
{
	for (LocationConfig &location_config : server_config.locations_confs)
	{
		locations.push_back(Location(location_config));
	}
}

void Server::listen()
{
	socket_fds.listen();
}

std::vector<int> Server::accept(void)
{
	return socket_fds.accept();
}