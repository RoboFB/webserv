/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 16:14:55 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/20 21:20:02 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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

		
          SocketFd(SocketFd &&other) : socket_fd_(other.socket_fd_) { other.socket_fd_ = -1; }
          SocketFd &operator=(SocketFd &&) = delete;

          SocketFd(const struct addrinfo *one_addr);
          ~SocketFd();


          void listen(void);

          int accept(void);

          int get_fd() const;

	private:
		int socket_fd_;
		static constexpr int backlog_ = 10;
};