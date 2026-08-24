/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/24 17:59:05 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AddrInfoPtr.hpp"
#include "Methods.hpp"
#include "CloseFd.hpp"
#include "SocketFds.hpp"
#include "ConfigStructs.hpp"

#include <filesystem>
#include <vector>

class Location
{ // clang-format off
	public:
		std::vector<int>                        error_codes; //todo
		std::filesystem::path                   error_page_path; //todo
		size_t                                  client_max_body_size;
		bool                                    autoindex;
		std::filesystem::path                   root_path;
		std::vector<std::filesystem::path>      indexs_paths;
		int                                     return_code; //todo
		std::filesystem::path                   return_path; //todo
		Methods 				                limit_except;

		Location(LocationConfig &location_config)
			: error_codes(location_config.error_codes),
				error_page_path(location_config.error_page_path.value()),
				client_max_body_size(location_config.client_max_body_size.value()),
				autoindex(location_config.autoindex.value()),
				root_path(location_config.root_path.value()),
				indexs_paths(location_config.indexs_paths),
				return_code(location_config.return_code.value()),
				return_path(location_config.return_path.value()),
				limit_except(location_config.limit_except) {}
}; // clang-format on

class Server
{
	public:
		Server(const Server &) = delete;
		Server &operator=(const Server &) = delete;

		Server(Server &&) = default;
		Server &operator=(Server &&) = delete;

		Server(ServerConfig &server_config);

		void add_to_epoll(const CloseFd &epoll_fd);

		void listen(void);
		std::vector<int> accept(void);

	private:
		std::vector<Location> locations_;
		AddrInfoPtr
			addr_listen_list_; // address:port linked list of listen addresses
		SocketFds socket_fds_; // socket file descriptor for the listen address
		std::vector<int>
			epoll_fds_; // epoll file descriptors for the listen address
};

class AllServers
{
	public:
		AllServers(MainConfig &main_config);
		void add_to_epoll(void);
		void wait_epoll(void);

		void listen(void);
		std::vector<int> accept(void);
		Server &at(size_t index);

	private:
		std::vector<Server> all_servers_;
		CloseFd epoll_fd_; // epoll file descriptor for all servers
};