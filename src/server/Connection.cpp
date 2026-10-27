/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/27 18:19:46 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "AllServer.hpp"
#include "logging.hpp"
#include "MagicValues.hpp"

#include <sys/epoll.h>
#include <array>
#include <cstring>
#include <cerrno>

Connection::Connection(const Server *server, CloseFd &&fd,
					   const CloseFd &epoll_fd)
	: EpollHandler(server, std::move(fd), epoll_fd), state_(State::RECEIVING),
	  last_activity_(std::chrono::steady_clock::now())
{
	this->add_to_epoll();
}

void Connection::queue_response()
{
	response_.serialize();
	state_ = State::SENDING;
	modify_epoll(EPOLL_EVENTS::EPOLLOUT);
}

void Connection::receive()
{
	static constexpr std::size_t buffer_size = 4096;
	std::array<uint8_t, buffer_size> temporary_buffer = {};
	const ssize_t bytes_read =
		::recv(fd_, temporary_buffer.data(), temporary_buffer.size(), 0);
	if (bytes_read == 0)
	{
		stop_listening();
		return;
	}
	if (bytes_read < 0)
	{
		// if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
		// 	return;
		LOG(LOG_ERROR, "recv: " + std::string(std::strerror(errno)));
		stop_listening();
		return;
	}

	request_.buffer_.insert(request_.buffer_.end(), temporary_buffer.begin(),
							temporary_buffer.begin() + bytes_read);
	const ParseStatus parsed = request_.request_parsing();
	if (parsed == ParseStatus::INCOMPLETE)
		return;
	if (parsed == ParseStatus::BAD)
	{
		response_ = make_error_response(400);
		queue_response();
		return;
	}
	request_.buffer_.clear();
	response_ = request_.handle_request(*server_);
	queue_response();
}

void Connection::send()
{
	if (response_.offset_ >= response_.buffer_.size())
	{
		stop_listening();
		return;
	}
	const ssize_t bytes_sent =
		::send(fd_, response_.buffer_.data() + response_.offset_,
			   response_.buffer_.size() - response_.offset_, MSG_NOSIGNAL);
	if (bytes_sent < 0)
	{
		// if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
		// 	return;
		LOG(LOG_ERROR, "send: " + std::string(std::strerror(errno)));
		stop_listening();
		return;
	}
	if (bytes_sent == 0)
	{
		stop_listening();
		return;
	}
	response_.offset_ += static_cast<std::size_t>(bytes_sent);
	if (response_.offset_ == response_.buffer_.size())
		stop_listening();
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
	last_activity_ = std::chrono::steady_clock::now();
}

bool Connection::is_timeout() const
{
	return std::chrono::steady_clock::now() - last_activity_ >
		   std::chrono::seconds(CONNECTION_TIMEOUT_S);
}

void Connection::stop_listening(void)
{
	state_ = State::FINISHED;
	if (remove_me_)
		return;
	remove_from_epoll();
	remove_me_ = true;
}
