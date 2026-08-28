/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 18:21:33 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Location.hpp"

#include "AddrInfoPtr.hpp"
#include "CloseFd.hpp"
#include "ConfigStructs.hpp"
#include "SocketFd.hpp"

#include <set>
#include <vector>

class Server
{
	public:
		Server(const Server &) = delete;
		Server &operator=(const Server &) = delete;

		Server(Server &&) = default;
		Server &operator=(Server &&) = delete;

		Server(ServerConfig &server_config);

		void add_to_epoll(const CloseFd &epoll_fd) const;

		void listen(void) const;
		std::vector<CloseFd> accept(void) const;

		bool smart_accept(const int compare_with_me);

	private:
		std::vector<Location> locations_;
		AddrInfoPtr addr_listen_list_; // linked list unique pointer to head
		std::set<SocketFd> socket_fds_;
		std::vector<CloseFd> accepted_fds_;
};
