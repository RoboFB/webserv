/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:55:07 by rgohrig           #+#    #+#             */
/*   Updated: 2026/11/01 21:16:07 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Location.hpp"
#include "Methods.hpp"

Response::Response() : Message("HTTP/1.1"), status_(500) {}

void Response::set_status(int status)
{
	if (status < 100 || status > 599)
		this->status_ = 500;
	else
		this->status_ = status;
}

std::string Response::getReason() const
{
	const std::map<int, std::string>::const_iterator it =
		reasons_.find(status_);

	if (it != reasons_.end())
		return it->second;

	return "Unknown Status";
}

static std::string allowed_methods(Methods methods)
{
	std::string result;

	if (is_method(methods, Methods::GET))
		result += "GET";
	if (is_method(methods, Methods::POST))
		result += (result.empty() ? "" : ", ") + std::string("POST");
	if (is_method(methods, Methods::DELETE))
		result += (result.empty() ? "" : ", ") + std::string("DELETE");

	return result;
}

void Response::convert_to_not_allowed(const Location &location)
{
	set_status(405);
	addHeader("Allow", allowed_methods(location.limit_except));
}

void Response::convert_to_error(int status)
{
	set_status(status);
	// version_ = "HTTP/1.1";
	body_ = std::to_string(status) + " " + getReason() + "\n";
	headers_.clear();
	addHeader("Content-Type", "text/plain; charset=utf-8");
}

void Response::serialize()
{
	headers_.set("Server", "webserv");
	headers_.set("Connection", "close");
	headers_.set("Content-Length", std::to_string(body_.size()));

	head_.clear();
	head_.reserve(version_.size() + getReason().size() + 64);

	head_ += version_;
	head_ += " ";
	head_ += std::to_string(status_);
	head_ += " ";
	head_ += getReason();
	head_ += LINE;

	headers_.serialize(head_);
	head_ += LINE;

	head_offset_ = 0;
	body_offset_ = 0;
}

const char *Response::send_data() const
{
	if (head_offset_ < head_.size())
		return head_.data() + head_offset_;
	if (body_offset_ < body_.size())
		return body_.data() + body_offset_;
	return nullptr;
}

std::size_t Response::send_size() const
{
	if (head_offset_ < head_.size())
		return head_.size() - head_offset_;
	if (offset_ < body_.size())
		return body_.size() - body_offset_;
	return 0;
}

void Response::consume_sent(std::size_t bytes)
{
	if (head_offset_ < head_.size())
	{
		head_offset_ += bytes;
		return;
	}
	body_offset_ += bytes;
}

bool Response::is_fully_sent() const
{
	return head_offset_ == head_.size() &&
		   body_offset_ == body_.size();
}

Response &Response::addHeader(const std::string &name, const std::string &value)
{
	Message::addHeader(name, value);
	return *this;
}
