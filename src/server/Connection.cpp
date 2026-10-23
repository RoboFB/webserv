/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/20 18:34:08 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "AllServer.hpp"
#include "logging.hpp"
#include "RequestParser.hpp"
#include "Message.hpp"

#include <sys/epoll.h>
#include <array>
#include <cstring>
#include <cerrno>

Connection::Connection(const Server *server, CloseFd &&fd,
					   const CloseFd &epoll_fd)
	: EpollHandler(server, std::move(fd), epoll_fd), state_(State::RECEIVING)
{
}

void Connection::finish()
{
	if (state_ == State::FINISHED)
		return;
	remove_from_epoll();
	set_remove_me();
	state_ = State::FINISHED;
}

void Connection::queue_response(const Response &response)
{
	const std::string serialized = response.serialize();
	response_buffer_.assign(serialized.begin(), serialized.end());
	response_offset_ = 0;
	state_ = State::SENDING;
	modify_epoll(EPOLLOUT);
}

void Connection::receive()
{
	static constexpr std::size_t buffer_size = 4096;
	std::array<uint8_t, buffer_size> temporary_buffer = {};
	const ssize_t bytes_read =
		::recv(fd_, temporary_buffer.data(), temporary_buffer.size(), 0);
	if (bytes_read == 0)
	{
		finish();
		return;
	}
	if (bytes_read < 0)
	{
		if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		LOG(LOG_ERROR, "recv: " + std::string(std::strerror(errno)));
		finish();
		return;
	}
	request_buffer_.insert(request_buffer_.end(), temporary_buffer.begin(),
						   temporary_buffer.begin() + bytes_read);
	const RequestParsing parsed = request_parsing(request_buffer_);
	if (parsed.status == ParseStatus::INCOMPLETE)
		return;
	if (parsed.status == ParseStatus::BAD)
	{
		queue_response(make_error_response(400));
		return;
	}
	request_buffer_.erase(request_buffer_.begin(),
						  request_buffer_.begin() + parsed.bytes);
	queue_response(parsed.message.handle_request(*server_));
}

Connection::~Connection() {}

void Connection::send()
{
	if (response_offset_ >= response_buffer_.size())
	{
		finish();
		return;
	}
	const std::size_t remaining = response_buffer_.size() - response_offset_;
	const ssize_t bytes_sent =
		::send(fd_, response_buffer_.data() + response_offset_, remaining, 0);
	if (bytes_sent < 0)
	{
		if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		LOG(LOG_ERROR, "send: " + std::string(std::strerror(errno)));
		finish();
		return;
	}
	if (bytes_sent == 0)
	{
		finish();
		return;
	}
	response_offset_ += static_cast<std::size_t>(bytes_sent);
	if (response_offset_ == response_buffer_.size())
		finish();
}

void Connection::on_epoll_event(AllServers &servers, uint32_t events)
{
	(void)servers;

	LOG(LOG_DEBUG, " state: " + std::to_string(static_cast<int>(state_)));
	switch (state_)
	{
	case State::RECEIVING:
		if (events & EPOLL_EVENTS::EPOLLIN)
			receive();
		break;
	case State::SENDING:
		if (events & EPOLL_EVENTS::EPOLLOUT)
			send();
		break;
	case State::FINISHED:
		break;
	}
}
