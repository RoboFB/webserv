/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFds.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 20:21:16 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/24 17:08:14 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AddrInfoPtr.hpp"
#include "CloseFd.hpp"
#include "SocketFd.hpp"
#include <vector>

class SocketFds
{
	public:
		SocketFds(const SocketFds &) = delete;
		SocketFds &operator=(const SocketFds &) = delete;

		SocketFds(SocketFds &&) = default;
		SocketFds &operator=(SocketFds &&) = delete;

		SocketFds(const AddrInfoPtr &start);

		void listen(void);

		std::vector<int> accept(void);

		void add_to_epoll(const CloseFd &epoll_fd, void *server);

		SocketFd &at(size_t index);

	private:
		std::vector<SocketFd> socket_fds_;
};
