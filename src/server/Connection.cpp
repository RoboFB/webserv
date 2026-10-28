/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/28 16:42:00 by rgohrig          ###   ########.fr       */
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
	: EpollHandler(server, std::move(fd), epoll_fd), cgi_(nullptr),
	  state_(State::READ_HEAD), last_activity_(std::chrono::steady_clock::now())
{
	add_to_epoll();
}

void Connection::receive()
{
	static constexpr std::size_t buffer_size = 4096;
	std::array<uint8_t, buffer_size> temporary_buffer = {};
	const ssize_t bytes_read =
		::recv(fd_, temporary_buffer.data(), temporary_buffer.size(), 0);
	if (bytes_read == 0)
	{
		set_state(State::FINISHED);
		return;
	}
	if (bytes_read < 0)
	{
		// if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
		// 	return;
		LOG(LOG_ERROR, "recv: " + std::string(std::strerror(errno)));
		set_state(State::FINISHED);
		return;
	}

	request_.buffer_.insert(request_.buffer_.end(), temporary_buffer.begin(),
							temporary_buffer.begin() + bytes_read);
	const ParseStatus parsed = request_.request_parsing();
	if (parsed == ParseStatus::INCOMPLETE)
		return;
	if (parsed == ParseStatus::BAD)
	{
		response_.convert_to_error(400);
		set_state(State::WRITE);
		return;
	}
	request_.buffer_.clear();
	handle_request(request_, response_, *server_);
	set_state(State::WRITE);
}

void Connection::send()
{
	if (response_.offset_ >= response_.buffer_.size())
	{
		set_state(State::FINISHED);
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
		set_state(State::FINISHED);
		return;
	}
	if (bytes_sent == 0)
	{
		set_state(State::FINISHED);
		return;
	}
	response_.offset_ += static_cast<std::size_t>(bytes_sent);
	if (response_.offset_ == response_.buffer_.size())
		set_state(State::FINISHED);
}

void Connection::on_epoll_event(AllServers &servers, uint32_t events)
{
	(void)servers;
	if (events & (EPOLLERR | EPOLLHUP))
		return set_state(State::FINISHED);

	switch (state_)
	{
	case State::READ_HEAD:
	case State::READ_BODY:
		if (events & EPOLLIN)
			receive();
		break;
	case State::WAIT_CGI: // wait for cgi to finish and then send response
		break;
	case State::WRITE:
		if (events & EPOLLOUT)
			send();
		break;
	case State::FINISHED: // should not happen, but if it does, just ignore it
		break;
	}

	// IN_TO_OUT has no epoll event to wait for, so run it right after the
	// read that finished the request
}

void Connection::set_state(State new_state)
{
	last_activity_ = std::chrono::steady_clock::now();
	state_ = new_state;
	switch (state_)
	{
	case State::READ_HEAD:
		modify_epoll(EPOLL_EVENTS::EPOLLIN);
		break;
	case State::READ_BODY:
		break;
	case State::WAIT_CGI:
		modify_epoll(EPOLL_EVENTS::EPOLLRDHUP);
		break;
	case State::WRITE:
		response_.serialize();
		modify_epoll(EPOLL_EVENTS::EPOLLOUT);
		break;
	case State::FINISHED:
		stop_listening();
		break;
	}
}

bool Connection::is_timeout() const
{
	return std::chrono::steady_clock::now() - last_activity_ >
		   std::chrono::seconds(CONNECTION_TIMEOUT_S);
}

void Connection::stop_listening(void)
{
	if (remove_me_)
		return;
	remove_from_epoll();
	remove_me_ = true;
}
