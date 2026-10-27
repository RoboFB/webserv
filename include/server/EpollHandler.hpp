/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 00:00:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/27 12:52:45 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include <cstdint>

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

		virtual void on_epoll_event(AllServers &servers, uint32_t events) = 0;
		virtual bool is_timeout() const = 0;
		virtual void stop_listening(void) = 0;

		bool is_remove_me(void) const;

	protected:
		const Server *server_;
		const CloseFd &epoll_fd_;
		CloseFd fd_;
		bool remove_me_;
		void add_to_epoll();
		void modify_epoll(uint32_t events);
		void remove_from_epoll() const;

	private:
};
