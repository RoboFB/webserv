/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 16:14:55 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/20 20:54:44 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketFd.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <cerrno>


SocketFd::SocketFd(const struct addrinfo *one_addr)
{
	socket_fd_ = ::socket(one_addr->ai_family, one_addr->ai_socktype, one_addr->ai_protocol);
	if (socket_fd_ < 0)
	{
		throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
	}

	if (::bind(socket_fd_, one_addr->ai_addr, one_addr->ai_addrlen) < 0)
	{
		int bind_errno = errno;
		char host[NI_MAXHOST];
		char port[NI_MAXSERV];
		::getnameinfo(one_addr->ai_addr, one_addr->ai_addrlen, host, sizeof(host),
			port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
		throw std::runtime_error("bind " + std::string(host) + ":" + port + ": " + std::strerror(bind_errno));
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