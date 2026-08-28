/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 16:14:55 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 20:03:26 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketFd.hpp"
#include "logging.hpp"

#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <cerrno>

// makes the socket calls socket(), bind()
SocketFd::SocketFd(const struct addrinfo *one_addr)
	: socket_fd_(::socket(one_addr->ai_family,
						  one_addr->ai_socktype | SOCK_NONBLOCK,
						  one_addr->ai_protocol))
{
	if (socket_fd_ < 0)
	{
		throw std::runtime_error(std::string("socket: ") +
								 std::strerror(errno));
	}

	if (::bind(socket_fd_, one_addr->ai_addr, one_addr->ai_addrlen) < 0)
	{
		int bind_errno = errno;
		char host[NI_MAXHOST];
		char port[NI_MAXSERV];
		::getnameinfo(one_addr->ai_addr, one_addr->ai_addrlen, host,
					  sizeof(host), port, sizeof(port),
					  NI_NUMERICHOST | NI_NUMERICSERV);
		throw std::runtime_error("bind " + std::string(host) + ":" + port +
								 ": " + std::strerror(bind_errno));
	}
}

SocketFd::SocketFd(SocketFd &&other) : socket_fd_(std::move(other.socket_fd_))
{
}

SocketFd::~SocketFd() {}

void SocketFd::listen(void) const
{
	if (::listen(socket_fd_, backlog_) < 0)
	{
		throw std::runtime_error(std::string("listen: ") +
								 std::strerror(errno));
	}
}

// returns the new socket file descriptor for the accepted connection,
// ignores the address of the connecting client.
int SocketFd::accept(void) const
{
	struct sockaddr_storage client_addr;
	socklen_t addr_len = sizeof(client_addr);

	int client_fd_ =
		::accept(socket_fd_, reinterpret_cast<struct sockaddr *>(&client_addr),
				 &addr_len);
	if (client_fd_ < 0)
	{
		LOG(LOG_ERROR, "accept: " + std::string(std::strerror(errno)));
		// throw std::runtime_error(std::string("accept: ") +
		// std::strerror(errno));
	}
	return client_fd_;
}

void SocketFd::add_to_epoll(const CloseFd &epoll_fd) const
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = socket_fd_;

	if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd_, &ev) < 0)
	{
		LOG(LOG_ERROR, "epoll_ctl: " + std::string(std::strerror(errno)));
		// throw std::runtime_error(std::string("epoll_ctl: ") +
		// 						 std::strerror(errno));
	}
	LOG(LOG_DEBUG, "Added socket_fd_ " + std::to_string(socket_fd_) +
					   " to epoll_fd_ " + std::to_string(epoll_fd));
}

bool SocketFd::operator==(int compare_with_me) const
{
	return socket_fd_ == compare_with_me;
}

bool SocketFd::operator<(const SocketFd &other) const
{
	return socket_fd_ < other.socket_fd_;
}
