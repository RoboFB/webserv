/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/11/02 12:36:07 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionHandler.hpp"
#include "AllServer.hpp"
#include "logging.hpp"
#include "MagicValues.hpp"
#include "Resolver.hpp"

#include <memory>
#include <system_error>
#include <sys/epoll.h>
#include <array>
#include <cstring>
#include <cerrno>

ConnectionHandler::ConnectionHandler(const Server *server, CloseFd &&fd,
									 const CloseFd &epoll_fd)
	: EpollHandler(server, std::move(fd), epoll_fd), cgi_(nullptr),
	location_(nullptr), state_(State::READ_HEAD),
	last_activity_(std::chrono::steady_clock::now())
{
	add_to_epoll();
}

void ConnectionHandler::receive(AllServers &servers)
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
		LOG(LOG_ERROR, "recv: " + std::string(std::strerror(errno)));
		set_state(State::FINISHED);
		return;
	}

	request_.buffer_.insert(request_.buffer_.end(), temporary_buffer.begin(),
							temporary_buffer.begin() + bytes_read);

	if (state_ == State::READ_HEAD)
		process_head();

	if (state_ == State::READ_BODY)
		process_body(servers);
}

void ConnectionHandler::process_head()
{
	const ParseStatus parsed = request_.parse_request_head();

	if (parsed == ParseStatus::INCOMPLETE)
		return;

	if (parsed == ParseStatus::BAD)
	{
		response_.convert_to_error(400);
		set_state(State::WRITE);
		return;
	}

	const URITarget target = request_.split_uri_target();

	location_ = server_->find_location(target.path);

	if (location_ == nullptr)
	{
		response_.convert_to_error(404);
		set_state(State::WRITE);
		return;
	}

	if (location_->redirect_page)
	{
		response_.convert_to_redirect(
			location_->redirect_page->first,
			location_->redirect_page->second);
		set_state(State::WRITE);
		return;
	}

	if (!is_method(location_->limit_except, request_.get_method()))
	{
		response_.convert_to_not_allowed(*location_);
		set_state(State::WRITE);
		return;
	}

	set_state(State::READ_BODY);
}

void ConnectionHandler::process_body(AllServers &servers)
{
    const ParseStatus parsed =
        request_.parse_body(location_->client_max_body_size);

    if (parsed == ParseStatus::INCOMPLETE)
        return;

    if (parsed == ParseStatus::TOO_LARGE)
    {
        response_.convert_to_error(413, location_);
        set_state(State::WRITE);
        return;
    }

    if (parsed == ParseStatus::BAD)
    {
        response_.convert_to_error(400, location_);
        set_state(State::WRITE);
        return;
    }

    request_.buffer_.clear();

    const URITarget target = request_.split_uri_target();
    const ResolvedPath resolved =
        resolve_resource_path(*location_, target.path);

    if ((request_.get_method() == Methods::GET ||
         request_.get_method() == Methods::POST) &&
        resolved.status == ResolveStatus::OK)
    {
        std::filesystem::path cgi_path = resolved.path;
        std::error_code error;
        std::filesystem::file_status status =
            std::filesystem::status(cgi_path, error);

        if (!error && std::filesystem::is_directory(status) &&
            !target.path.empty() && target.path.back() == '/')
        {
            const std::optional<std::filesystem::path> index =
                find_index_file(*location_, cgi_path);

            if (index)
            {
                cgi_path = *index;
                error.clear();
                status = std::filesystem::status(cgi_path, error);
            }
        }

        const CGIConfigMap::const_iterator interpreter =
            location_->cgi_interpers.find(cgi_path.extension().string());

        if (!error &&
            std::filesystem::is_regular_file(status) &&
            interpreter != location_->cgi_interpers.end())
        {
            std::unique_ptr<CgiHandler> cgi = std::make_unique<CgiHandler>(
                server_, epoll_fd_, *this, response_, request_,
                cgi_path, interpreter->second);

            if (!cgi->is_started())
            {
                response_.convert_to_error(502, location_);
                set_state(State::WRITE);
                return;
            }

            cgi_ = cgi.get();
            servers.add_handler(std::move(cgi));
            set_state(State::WAIT_CGI);
            return;
        }
    }

    handle_request(request_, response_, *location_);
    set_state(State::WRITE);
}

void ConnectionHandler::send()
{
	if (response_.is_fully_sent())
	{
		set_state(State::FINISHED);
		return;
	}

	const ssize_t bytes_sent =
		::send(fd_, response_.send_data(), response_.send_size(),
			   MSG_NOSIGNAL);

	if (bytes_sent < 0)
	{
		LOG(LOG_ERROR, "send: " + std::string(std::strerror(errno)));
		set_state(State::FINISHED);
		return;
	}
	if (bytes_sent == 0)
	{
		set_state(State::FINISHED);
		return;
	}

	response_.consume_sent(static_cast<std::size_t>(bytes_sent));

	if (response_.is_fully_sent())
		set_state(State::FINISHED);
}

void ConnectionHandler::on_epoll_event(AllServers &servers, uint32_t events)
{
	// if (events & (EPOLLERR | EPOLLHUP))
	// 	return set_state(State::FINISHED);
	if (events & EPOLLERR)
	{
		set_state(State::FINISHED);
		return;
	}

	switch (state_)
	{
	case State::READ_HEAD:
	case State::READ_BODY:
		if (events & (EPOLLIN | EPOLLRDHUP))
			receive(servers);
		break;
	case State::WAIT_CGI:
		if (events & (EPOLLRDHUP | EPOLLHUP))
			set_state(State::FINISHED);
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

void ConnectionHandler::set_state(State new_state)
{
	last_activity_ = std::chrono::steady_clock::now();
	state_ = new_state;
	switch (state_)
	{
	case State::READ_HEAD:
		modify_epoll(EPOLL_EVENTS::EPOLLIN);
		break;
	case State::READ_BODY:
		modify_epoll(EPOLL_EVENTS::EPOLLIN);
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

bool ConnectionHandler::is_timeout() const
{
	return std::chrono::steady_clock::now() - last_activity_ >
		   std::chrono::seconds(CONNECTION_TIMEOUT_S);
}

void ConnectionHandler::stop_listening(void)
{
	if (remove_me_)
		return;
	if (cgi_ != nullptr)
	{
		cgi_->stop_listening();
		cgi_ = nullptr;
	}
	remove_from_epoll();
	remove_me_ = true;
}

void ConnectionHandler::cgi_finished()
{
    cgi_ = nullptr;
    set_state(State::WRITE);
}
