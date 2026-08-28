/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 20:29:16 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include <string>
#include <sys/types.h>

// wraps an already-accepted client fd (from SocketFd::accept()).
// unlike SocketFd, a Connection never listens/accepts, it only reads/writes
// on a fd that already refers to a connected client.
class Connection
{
	public:
		Connection(const Connection &) = delete;
		Connection &operator=(const Connection &) = delete;

		Connection(Connection &&) = default;
		Connection &operator=(Connection &&) = default;

		Connection(CloseFd &&connection_fd);
		~Connection();

		void add_to_epoll(const CloseFd &epoll_fd) const;
		void send(const std::string &message) const;

		// reads once into an internal buffer and logs it (placeholder until
		// request parsing is wired in). returns what read() returned:
		// >0 bytes read, 0 peer closed, <0 error.
		ssize_t receive(void) const;

		bool operator==(int compare_with_me) const;

	private:
		CloseFd connection_fd_;
};