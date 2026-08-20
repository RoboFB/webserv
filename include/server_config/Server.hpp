/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/20 21:33:05 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AddrInfoPtr.hpp"
#include "Methods.hpp"
#include "SocketFds.hpp"
#include <filesystem>
#include <vector>

#include "ConfigStructs.hpp"

class Location
{
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
				client_max_body_size(
					location_config.client_max_body_size.value()),
				autoindex(location_config.autoindex.value()),
				root_path(location_config.root_path.value()),
				indexs_paths(location_config.indexs_paths),
				return_code(location_config.return_code.value()),
				return_path(location_config.return_path.value()),
				limit_except(location_config.limit_except) {}
};

class Server
{
	public:
		Server(const Server &) = delete;
		Server &operator=(const Server &) = delete;


		
		Server(Server &&) = default;
		Server &operator=(Server &&) = delete;

		


		Server(ServerConfig &server_config);

		void listen(void);
		std::vector<int> accept(void);

	private:
		std::vector<Location> locations;
		AddrInfoPtr addr_listen_list; // address:port linked list of listen addresses
		SocketFds socket_fds; // socket file descriptor for the listen address

	

};


class AllServers
{
	public:
		AllServers(MainConfig &main_config);
		void listen(void);
		std::vector<int> accept(void);
		Server &at(size_t index);
	private:
		std::vector<Server> all_servers;
};