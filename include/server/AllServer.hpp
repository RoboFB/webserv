/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 13:07:50 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include "EpollHandler.hpp"
#include "ConfigParserStructs.hpp"
#include "Server.hpp"

#include <vector>

class AllServers
{
	public:
		AllServers(const AllServers &) = delete;
		AllServers(AllServers &&) = delete;
		AllServers &operator=(const AllServers &) = delete;
		AllServers &operator=(AllServers &&) = delete;

		AllServers(MainConfigParser &&main_config);
		~AllServers(); // calls stop_all()

		Server &at(size_t index);

		void wait_epoll(void);
		// adds to all_fds_
		void add_handler(std::unique_ptr<EpollHandler> &&new_handler);

		void check_timeouts(void);
		void cleanup_all_fds(void);

		// const CloseFd &get_epoll_fd(void) const;

	private:
		int init_epoll(void) const;
		void stop_all(void);

		std::vector<Server> all_servers_;
		CloseFd epoll_fd_; // epoll file descriptor for all servers
		std::vector<std::unique_ptr<EpollHandler>> all_fds_;
};