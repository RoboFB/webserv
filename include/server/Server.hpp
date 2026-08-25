/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/25 14:24:36 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Location.hpp"

#include "AddrInfoPtr.hpp"
#include "CloseFd.hpp"
#include "ConfigStructs.hpp"
#include "SocketFd.hpp"

#include <vector>

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
		AddrInfoPtr addr_listen_list_; // linked list unique pointer to head
		std::vector<SocketFd> socket_fds_;
};
