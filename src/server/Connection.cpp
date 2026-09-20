/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/21 01:02:08 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "AllServer.hpp"
#include "logging.hpp"
#include "RequestParser.hpp"

#include <sys/epoll.h>
#include <unistd.h>
#include <array>
#include <cstring>

Connection::Connection(const Server *server, CloseFd &&fd,
					   const CloseFd &epoll_fd)
	: EpollHandler(server, std::move(fd), epoll_fd), state_(State::RECEIVING)
{
}

// void Connection::receive(void)
// {
// 	static constexpr size_t buffer_size = 4096;

// 	std::array<uint8_t, buffer_size> tmp_buffer = {};

// 	ssize_t bytes_read = ::recv(fd_, tmp_buffer.data(), buffer_size, 0);

// 	if (bytes_read <= -1)
// 	{
// 		LOG(LOG_ERROR, "recv: " + std::string(std::strerror(errno)));
// 	}
// 	else if (bytes_read == buffer_size) // still more to read
// 	{
// 		request_buffer_.insert(request_buffer_.end(), tmp_buffer.begin(),
// 							   tmp_buffer.end());
// 		return;
// 	}
// 	else if (bytes_read == 0)
// 	{
// 		LOG(LOG_DEBUG,
// 			"Connection::receive: peer closed fd " + std::to_string(fd_));
// 	}
// 	else // read completed
// 	{
// 		request_buffer_.insert(request_buffer_.end(), tmp_buffer.begin(),
// 							   tmp_buffer.begin() + bytes_read);
// 	}

// 	if (request_buffer_.empty())
// 	{
// 		remove_from_epoll();
// 		set_remove_me();
// 		return;
// 	}
// 	modify_epoll(EPOLL_EVENTS::EPOLLOUT);
// 	state_ = State::BUILTING;

// 	return;
// }

Connection::receive(void) {
	static constexpr size_t buffer_size = 4096;
	std::array<uint8_t, buffer_size> tmp_buffer = {};
	ssize_t bytes_read = ::recv(fd_, tmp_buffer.data(), buffer_size, 0);
	if (bytes_read == 0) {
		remove_from_epoll();
		set_remove_me();
		return;
	}
	if (bytes_read < 0) {
		remove_from_epoll();
		set_remove_me();
		return;
	}
	request_buffer_.insert(request_buffer_.end(), tmp_buffer.begin(), tmp_buffer.begin() + bytes_read);
	const RequestParsing parsed = request_parsing(request_buffer_);
	if (parsed.status == RequestStatus::MSG_INCOMPLETE)
		return;
	if (parsed.status == RequestStatus::MSG_BAD) {
		//error 400
		state_ = State::SENDING;
		modify_epoll(EPOLLOUT);
		return;
	}
	request_buffer_.erase(request_buffer_.begin(), request_buffer_.begin() + parsed.bytes);
	make_response(parsed.message);
	state_ = State::SENDING;
	modify_epoll(EPOLLOUT);
}

Connection::~Connection() {}

void Connection::send()
{
	if (::send(fd_, response_buffer_.data(), response_buffer_.size(), 0) < 0)
	{
		throw std::runtime_error(std::string("send: ") + std::strerror(errno));
	}
	remove_from_epoll();
	set_remove_me();
	state_ = State::FINISHED;
}

std::string response2 =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n\r\n"
	"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
	"<style>body { background-color: #111 }"
	"h1 { font-size:4cm; text-align: center; color: black;"
	" text-shadow: 0 0 2mm red}</style></head>"
	"<body><h1>Goodbye, world!</h1></body></html>\r\n\r\n";

void Connection::make_response(const Request& request)
{
	// for (auto &byte : response2)
	// {
	// 	response_buffer_.push_back(static_cast<uint8_t>(byte));
	// }
	// state_ = State::SENDING;
	const Response response = handle_request(request, *server_);
	const std::string	send = response.serialize();
	response_buffer_.assign(send.begin(), send.end());
	response_offset_ = 0;
	return;
}

void Connection::on_epoll_event(AllServers &servers, uint32_t events)
{
	(void)servers;

	LOG(LOG_DEBUG, " state: " + std::to_string(static_cast<int>(state_)));

	switch (state_)
	{
	case State::RECEIVING:
		if (events & EPOLL_EVENTS::EPOLLIN)
		{
			receive();
			return;
		}
		break;

	case State::BUILTING:
		if (events & EPOLL_EVENTS::EPOLLOUT)
		{
			make_response();
			return;
		}
		break;

	case State::SENDING:
		if (events & EPOLL_EVENTS::EPOLLOUT)
		{
			send();
			return;
		}
		break;
	case State::FINISHED:
		break;
	}
}