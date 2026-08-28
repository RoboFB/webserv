/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 20:32:20 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "logging.hpp"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <stdexcept>

Connection::Connection(CloseFd &&client_fd)
	: connection_fd_(std::move(client_fd))
{
}

Connection::~Connection() {}

void Connection::add_to_epoll(const CloseFd &epoll_fd) const
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = connection_fd_;

	if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_fd_, &ev) < 0)
	{
		LOG(LOG_ERROR, "epoll_ctl: " + std::string(std::strerror(errno)));
	}
	LOG(LOG_DEBUG, "Added client_fd_ " + std::to_string(connection_fd_) +
					   " to epoll_fd_ " + std::to_string(epoll_fd));
}

void Connection::send(const std::string &message) const
{
	if (::send(connection_fd_, message.c_str(), message.length(), 0) < 0)
	{
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));
	}
}

// ssize_t Connection::receive(void) const
// {
// 	static constexpr size_t buffer_size = 4096;
// 	std::array<char, buffer_size> buffer;

// 	ssize_t bytes_read = ::read(connection_fd_, buffer.data(), buffer.size());
// 	if (bytes_read > 0)
// 	{
// 		LOG(LOG_DEBUG, "Connection::receive: read " +
// 						   std::to_string(bytes_read) + " bytes from fd " +
// 						   std::to_string(connection_fd_));
// 	}
// 	else if (bytes_read == 0)
// 	{
// 		LOG(LOG_DEBUG, "Connection::receive: peer closed fd " +
// 						   std::to_string(connection_fd_));
// 	}
// 	else
// 	{
// 		LOG(LOG_ERROR, "read: " + std::string(std::strerror(errno)));
// 	}
// 	return bytes_read;
// }

bool Connection::operator==(int compare_with_me) const
{
	return connection_fd_ == compare_with_me;
}