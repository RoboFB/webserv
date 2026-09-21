/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/21 02:02:15 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Location.hpp"

#include "AddrInfoPtr.hpp"
#include "ConfigStructs.hpp"
#include "EpollHandler.hpp"

#include <vector>

class Server
{
	public:
		Server(Server &&) = default;
		Server(ServerConfig &server_config);

		Server(const Server &) = delete;
		Server &operator=(const Server &) = delete;
		Server &operator=(Server &&) = delete;

		const Location* find_location(const std::string& request_path) const;

		void add_sockets(std::vector<std::unique_ptr<EpollHandler>> &all_fds,
						 const CloseFd &epoll_fd) const;

	private:
		std::vector<Location> locations_;
		AddrInfoPtr addr_listen_list_; // linked list unique pointer to head,
									   // mostly for setup
};
