/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 16:14:55 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/02 19:52:21 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include "EpollHandler.hpp"
#include "Connection.hpp"

#include <memory>

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
class SocketFd : public EpollHandler
{
	public:
		SocketFd(const SocketFd &) = delete;
		SocketFd &operator=(const SocketFd &) = delete;

		SocketFd(SocketFd &&other) = delete;
		SocketFd &operator=(SocketFd &&) = delete;

		SocketFd(const struct addrinfo *one_addr, const Server *server,
				 const CloseFd &epoll_fd);
		~SocketFd() override;

		void listen(void) const;
		std::unique_ptr<Connection> accept(void) const;

		void on_epoll_event(AllServers &servers) override;

	private:
		static constexpr int backlog_ = 10;

		CloseFd init_socket(const struct addrinfo *one_addr) const;

		void accept_connection(AllServers &servers) const;
};
