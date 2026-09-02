/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 00:00:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/02 19:05:14 by rgohrig          ###   ########.fr       */
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

		EpollHandler(EpollHandler &&) = delete;
		EpollHandler &operator=(EpollHandler &&) = delete;

		EpollHandler(const Server *server, CloseFd &&fd,
					 const CloseFd &epoll_fd);
		virtual ~EpollHandler() = default;

		virtual void on_epoll_event(AllServers &servers) = 0;

		void add_to_epoll() const;
		void remove_from_epoll() const;

		bool is_remove_me(void) const;
		void set_remove_me(void);

	protected:
		const Server *server_;
		const CloseFd &epoll_fd_;
		CloseFd fd_;

	private:
		bool remove_me_;
};
