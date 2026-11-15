/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 00:00:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/11/15 00:45:33 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include <cstdint>

class AllServers;
class Server;

class EpollHandler;

enum class EpollEndpoint { Primary, CgiInput };

struct EpollEventTarget
{
    EpollHandler *handler;
    EpollEndpoint endpoint;
};

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

		virtual void on_epoll_event(AllServers &servers, uint32_t events, EpollEndpoint endpoint) = 0;
		virtual bool is_timeout() const = 0;
		virtual void on_timeout() { stop_listening(); }
		virtual void stop_listening(void) = 0;

		bool is_remove_me(void) const;

		virtual void poll_child() {}
		virtual bool can_destroy() const { return is_remove_me(); }

	protected:
		const Server *server_;
		const CloseFd &epoll_fd_;
		CloseFd fd_;
		EpollEventTarget event_target_{this, EpollEndpoint::Primary};
		bool remove_me_;
		bool add_to_epoll();
		void modify_epoll(uint32_t events);
		void remove_from_epoll() const;

	private:
};
