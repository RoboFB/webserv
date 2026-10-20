/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 11:23:43 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/20 19:14:18 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

Message::Message() {}

Message::~Message() {}

Message &Message::setVersion(const std::string &version)
{
	this->version_ = version;
	return *this;
}

const std::string &Message::getVersion() const
{
	return version_;
}

Message &Message::addHeader(const std::string &name, const std::string &value)
{
	headers_[name].push_back(value);
	return *this;
}

const std::map<std::string, std::vector<std::string>> &
Message::getHeaders() const
{
	return headers_;
}

void Message::setBody(const std::string &body)
{
	this->body_ = body;
}

const std::string &Message::getBody() const
{
	return body_;
}

std::string Message::serializeHeaders() const
{
	std::string result;

	for (const auto &header : headers_)
	{
		for (const std::string &value : header.second)
			result += header.first + ": " + value + LINE;
	}
	return result;
}

Request::Request() : method_(Methods::GET), target_("/") {}

Request::Request(Methods method, const std::string &target)
	: method_(method), target_(target)
{
}

URITarget Request::split_uri_target() const
{
	const std::size_t query_start = target_.find('?');

	if (query_start == std::string::npos)
		return URITarget{target_, ""};

	return URITarget{target_.substr(0, query_start),
					 target_.substr(query_start + 1)};
}

std::string Request::serialize() const
{
	return methods_to_string(method_) + " " + target_ + " " + version_ + LINE +
		   serializeHeaders() + LINE + body_;
}

Response::Response() : status_(500) {}

Response::Response(int status)
{
	setStatus(status);
}

void Response::setStatus(int status)
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

std::string Response::serialize() const
{
	return version_ + " " + std::to_string(status_) + " " + getReason() + LINE +
		   serializeHeaders() + LINE + body_;
}

Response &Response::addHeader(const std::string &name, const std::string &value)
{
	Message::addHeader(name, value);
	return *this;
}
