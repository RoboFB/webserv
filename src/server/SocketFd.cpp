/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 16:14:55 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/27 14:47:46 by rgohrig          ###   ########.fr       */
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
	: EpollHandler(server, -1, epoll_fd)
{
	this->init_socket(one_addr);
	this->init_setsockopt();
	this->init_bind(one_addr);
	this->init_listen();
	this->add_to_epoll();
}

void SocketFd::init_socket(const struct addrinfo *one_addr)
{
	fd_ = ::socket(one_addr->ai_family, one_addr->ai_socktype | SOCK_NONBLOCK,
				   one_addr->ai_protocol);
	if (fd_ < 0)
		throw std::runtime_error(std::string("socket: ") +
								 std::strerror(errno));
}

void SocketFd::init_setsockopt(void)
{
	const int enable_reuse = true;
	if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable_reuse,
					 sizeof(enable_reuse)) < 0)
		throw std::runtime_error(std::string("setsockopt: ") +
								 std::strerror(errno));
}

void SocketFd::init_bind(const struct addrinfo *one_addr)
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
void SocketFd::init_listen(void) const
{
	if (::listen(fd_, SOMAXCONN) < 0)
	{
		throw std::runtime_error(std::string("listen: ") +
								 std::strerror(errno));
	}
}

void SocketFd::accept(AllServers &servers) const
{
	struct sockaddr_storage client_addr;
	socklen_t addr_len = sizeof(client_addr);

	CloseFd client_fd_ = ::accept(
		fd_, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_len);
	if (client_fd_ < 0)
	{
		LOG(LOG_DEBUG, "accept: " + std::string(std::strerror(errno)));
		return;
	}
	servers.add_handler(std::make_unique<Connection>(
		server_, std::move(client_fd_), epoll_fd_));
	return;
}

void SocketFd::on_epoll_event(AllServers &servers, uint32_t events)
{
	if (!(events & EPOLL_EVENTS::EPOLLIN))
		return;
	accept(servers);
}

bool SocketFd::is_timeout() const
{
	return false;
}

void SocketFd::stop_listening(void) {}
