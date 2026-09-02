/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/02 19:11:39 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include "EpollHandler.hpp"
#include "ConfigStructs.hpp"
#include "Server.hpp"

#include <vector>

class AllServers
{
	public:
		AllServers(MainConfig &main_config);

		Server &at(size_t index);

		void wait_epoll(void);
		// adds to all_fds_
		void add_handel(std::unique_ptr<EpollHandler> &&new_handel);

		void cleanup_all_fds(void);

		// const CloseFd &get_epoll_fd(void) const;

	private:
		int init_epoll(void);

		std::vector<Server> all_servers_;
		CloseFd epoll_fd_; // epoll file descriptor for all servers
		std::vector<std::unique_ptr<EpollHandler>> all_fds_;
};