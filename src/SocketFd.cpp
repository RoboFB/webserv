/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 16:14:55 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/18 18:08:31 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketFd.hpp"

#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <cerrno>


SocketFd::SocketFd(const AddrInfoPtr &res) : socket_fd_(-1)
{
	socket_fd_ = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socket_fd_ < 0) {
		throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
	}

	if (::bind(socket_fd_, res->ai_addr, res->ai_addrlen) < 0)
	{
		throw std::runtime_error(std::string("bind: ") + std::strerror(errno));
	}

	
}

SocketFd::~SocketFd()
{
	if (socket_fd_ >= 0)
	{
		::close(socket_fd_);
	}
}


void SocketFd::listen(void)
{
	if (::listen(socket_fd_, backlog_) < 0)
	{
		throw std::runtime_error(std::string("listen: ") + std::strerror(errno));
	}
}

// returns the new socket file descriptor for the accepted connection,
// ignores the address of the connecting client.
int SocketFd::accept(void)
{
	struct sockaddr_storage client_addr;
	socklen_t addr_len = sizeof(client_addr);

	int client_fd_ = ::accept(socket_fd_, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_len);
	if (client_fd_ < 0)
	{
		throw std::runtime_error(std::string("accept: ") + std::strerror(errno));
	}
	return client_fd_;
}


int SocketFd::get_fd() const
{
	return socket_fd_;
}