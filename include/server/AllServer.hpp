/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 20:07:52 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include "ConfigStructs.hpp"
#include "Connection.hpp"
#include "Server.hpp"
#include "SocketFd.hpp"

#include <vector>

class AllServers
{
	public:
		AllServers(MainConfig &main_config);
		void add_to_epoll(SocketFd &connection_fd);

		Server &at(size_t index);

		void wait_epoll(void);

	private:
		void init_epoll(void);
		bool smart_accept(const int compare_with_me);
		bool try_receive(const int compare_with_me);

		std::vector<Server> all_servers_;
		CloseFd epoll_fd_; // epoll file descriptor for all servers

		std::vector<SocketFd> listen_fds_;
		std::vector<Connection> accepted_fds_;
};