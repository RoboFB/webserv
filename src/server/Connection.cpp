/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/02 19:06:12 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "AllServer.hpp"
#include "logging.hpp"

#include <unistd.h>
#include <array>
#include <cstring>

Connection::Connection(const Server *server, CloseFd &&fd,
					   const CloseFd &epoll_fd)
	: EpollHandler(server, std::move(fd), epoll_fd)
{
}

Connection::~Connection() {}

void Connection::send(const std::string &message) const
{
	if (::send(fd_, message.c_str(), message.length(), 0) < 0)
	{
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));
	}
}

ssize_t Connection::receive(void) const
{
	static constexpr size_t buffer_size = 4096;
	std::array<char, buffer_size> buffer;

	ssize_t bytes_read = ::read(fd_, buffer.data(), buffer.size());
	if (bytes_read > 0)
	{
		LOG(LOG_DEBUG, "Connection::receive: read " +
						   std::to_string(bytes_read) + " bytes from fd " +
						   std::to_string(fd_));
	}
	else if (bytes_read == 0)
	{
		LOG(LOG_DEBUG,
			"Connection::receive: peer closed fd " + std::to_string(fd_));
	}
	else
	{
		LOG(LOG_ERROR, "read: " + std::string(std::strerror(errno)));
	}
	return bytes_read;
}

std::string response2 =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n\r\n"
	"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
	"<style>body { background-color: #111 }"
	"h1 { font-size:4cm; text-align: center; color: black;"
	" text-shadow: 0 0 2mm red}</style></head>"
	"<body><h1>Goodbye, world!</h1></body></html>\r\n\r\n";

void Connection::on_epoll_event(AllServers &servers)
{
	// testing:
	send(response2);

	// todo:
	if (receive() <= 0)
	{
		remove_from_epoll();
		set_remove_me();
		(void)servers;
	}
}
