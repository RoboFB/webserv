/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 16:14:55 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 18:39:53 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"

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
class SocketFd
{
	public:
		SocketFd(const SocketFd &) = delete;
		SocketFd &operator=(const SocketFd &) = delete;

		SocketFd(SocketFd &&other);
		SocketFd &operator=(SocketFd &&) = delete;

		SocketFd(const struct addrinfo *one_addr);
		~SocketFd();

		void listen(void) const;

		int accept(void) const;

		void add_to_epoll(const CloseFd &epoll_fd) const;

		bool operator==(int compare_with_me) const;
		bool operator<(const SocketFd &other) const;

	private:
		CloseFd socket_fd_;
		static constexpr int backlog_ = 10;
};
