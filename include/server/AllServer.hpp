/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/29 10:45:03 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include "ConfigStructs.hpp"
#include "Connection.hpp"
#include "Server.hpp"
#include "SocketFd.hpp"

#include <vector>
#include <unordered_map>

class AllServers
{
	public:
		AllServers(MainConfig &main_config);

		Server &at(size_t index);

		void wait_epoll(void);

		// called by SocketFd::on_epoll_event once epoll reports it readable.
		void accept_connection(SocketFd &listener);
		// called by Connection::on_epoll_event once the peer closes/errors.
		void close_connection(int fd);

	private:
		void init_epoll(void);

		std::vector<Server> all_servers_;
		CloseFd epoll_fd_; // epoll file descriptor for all servers

		std::vector<SocketFd> listen_fds_;
		std::unordered_map<int, Connection> accepted_fds_;
};