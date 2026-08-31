/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 00:00:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/31 20:25:35 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"

class SocketFd;
class AllServers;
class Server;

class EpollHandler
{
	public:
		EpollHandler(const EpollHandler &) = delete;
		EpollHandler &operator=(const EpollHandler &) = delete;

		EpollHandler(EpollHandler &&) = default;
		EpollHandler &operator=(EpollHandler &&) = default;

		EpollHandler(const Server *server, CloseFd &&fd);
		virtual ~EpollHandler() = default;

		virtual void on_epoll_event(AllServers &servers) = 0;

		void add_to_epoll(const CloseFd &epoll_fd) const;

	protected:
		const Server *server_;
		CloseFd fd_;
};
