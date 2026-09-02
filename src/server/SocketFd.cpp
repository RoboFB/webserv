/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 16:14:55 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/02 19:51:38 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketFd.hpp"
#include "AllServer.hpp"
#include "logging.hpp"

#include "Connection.hpp"

#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <cerrno>

// makes the socket calls socket(), bind()
SocketFd::SocketFd(const struct addrinfo *one_addr, const Server *server,
				   const CloseFd &epoll_fd)
	: EpollHandler(server, init_socket(one_addr), epoll_fd)
{
	if (::bind(fd_, one_addr->ai_addr, one_addr->ai_addrlen) < 0)
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

CloseFd SocketFd::init_socket(const struct addrinfo *one_addr) const
{
	CloseFd init_fd =
		::socket(one_addr->ai_family, one_addr->ai_socktype | SOCK_NONBLOCK,
				 one_addr->ai_protocol);
	if (init_fd < 0)
	{
		throw std::runtime_error(std::string("socket: ") +
								 std::strerror(errno));
	}
	return init_fd;
}

SocketFd::~SocketFd() {}

void SocketFd::listen(void) const
{
	if (::listen(fd_, backlog_) < 0)
	{
		throw std::runtime_error(std::string("listen: ") +
								 std::strerror(errno));
	}
}

// returns the new socket file descriptor for the accepted connection,
// ignores the address of the connecting client.
std::unique_ptr<Connection> SocketFd::accept(void) const
{
	struct sockaddr_storage client_addr;
	socklen_t addr_len = sizeof(client_addr);

	CloseFd client_fd_ = ::accept(
		fd_, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_len);
	if (client_fd_ < 0)
	{
		LOG(LOG_ERROR, "accept: " + std::string(std::strerror(errno)));
		// throw std::runtime_error(std::string("accept: ") +
		// std::strerror(errno));
	}
	return std::make_unique<Connection>(server_, std::move(client_fd_),
										epoll_fd_);
}

void SocketFd::on_epoll_event(AllServers &servers)
{
	std::unique_ptr<Connection> new_connection = accept();
	new_connection->add_to_epoll();
	servers.add_handel(std::move(new_connection));
}
