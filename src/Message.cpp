/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 11:23:43 by modiepge          #+#    #+#             */
/*   Updated: 2026/08/12 18:34:30 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"



Message::Message() {}

Message::~Message() {}

void Message::setVersion(const std::string &version)
{
	this->version_ = version;
}

const std::string &Message::getVersion() const
{
	return version_;
}

void Message::addHeader(const std::string &name, const std::string &value)
{
	headers_[name].push_back(value);
}

const std::map<std::string, std::vector<std::string>> &Message::getHeaders() const
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
	: method_(method), target_(target) {}

void Request::setMethod(Methods method)
{
	this->method_ = method;
}

Methods Request::getMethod() const
{
	return method_;
}

void Request::setTarget(const std::string &target)
{
	this->target_ = target;
}

std::filesystem::path Request::getTarget() const
{
	return target_;
}

std::string Request::serialize() const
{
	return methods_to_string(method_) + " " + target_.string() + " " + version_ + LINE
		+ serializeHeaders() + LINE + body_;
}

Response::Response() {}

Response::Response(int status)
{
	setStatus(status);
}

void Response::setStatus(int status)
{
	if (reasons.find(status) == reasons.end())
		this->status_ = 500;
	else
		this->status_ = status;
}

int Response::getStatus() const
{
	return status_;
}

std::string Response::getReason() const
{
	std::map<int, std::string>::const_iterator it = reasons.find(status_);

	if (it == reasons.end())
		return reasons.at(500);
	return it->second;
}

std::string Response::serialize() const
{
	return version_ + " " + std::to_string(status_) + " " + getReason() + LINE
		+ serializeHeaders() + LINE + body_;
}
