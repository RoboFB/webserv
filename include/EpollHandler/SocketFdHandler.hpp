/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 16:14:55 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 13:12:34 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include "EpollHandler.hpp"

/*
init:
	1. creates a socket file descriptor
	2. binds to it.

needs to:
	1. call listen() to start listening for incoming connections.
	2. call new_fd = accept() to accept incoming connections.

deconstructs:
	1. closes the socket file descriptor.
*/
class SocketFdHandler : public EpollHandler
{
	public:
		SocketFdHandler(const SocketFdHandler &) = delete;
		SocketFdHandler &operator=(const SocketFdHandler &) = delete;

		SocketFdHandler(SocketFdHandler &&other) = delete;
		SocketFdHandler &operator=(SocketFdHandler &&) = delete;

		SocketFdHandler(const struct addrinfo *one_addr, const Server *server,
						const CloseFd &epoll_fd);
		~SocketFdHandler() override = default;

		void on_epoll_event(AllServers &servers, uint32_t events) override;
		bool is_timeout() const override;
		void stop_listening(void) override;

	private:
		void init_socket(const struct addrinfo *one_addr);
		void init_setsockopt(void);
		void init_bind(const struct addrinfo *one_addr);
		void init_listen(void) const;

		void accept(AllServers &servers) const;

		// void accept_connection(AllServers &servers) const;
};
