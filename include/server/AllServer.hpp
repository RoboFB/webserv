/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 17:30:13 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include "ConfigStructs.hpp"
#include "Server.hpp"

#include <vector>

class AllServers
{
	public:
		AllServers(MainConfig &main_config);
		void add_to_epoll(void);
		void wait_epoll(void);

		void listen(void) const;
		std::vector<CloseFd> accept(void) const;
		Server &at(size_t index);

	private:
		std::vector<Server> all_servers_;
		CloseFd epoll_fd_; // epoll file descriptor for all servers
};