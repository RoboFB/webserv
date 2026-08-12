/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 11:23:43 by modiepge          #+#    #+#             */
/*   Updated: 2026/08/12 18:11:00 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

static std::string methodToString(method_e method)
{
	switch (method)
	{
		case GET:
			return "GET";
		case HEAD:
			return "HEAD";
		case POST:
			return "POST";
		case PUT:
			return "PUT";
		case DELETE:
			return "DELETE";
		case CONNECT:
			return "CONNECT";
		case OPTIONS:
			return "OPTIONS";
		case TRACE:
			return "TRACE";
		case PATCH:
			return "PATCH";
	}
	return "GET";
}

Message::Message() {}

Message::~Message() {}

void Message::setVersion(const std::string &version)
{
	this->version = version;
}

const std::string &Message::getVersion() const
{
	return version;
}

void Message::addHeader(const std::string &name, const std::string &value)
{
	headers[name].push_back(value);
}

const std::map<std::string, std::vector<std::string>> &Message::getHeaders() const
{
	return headers;
}

void Message::setBody(const std::string &body)
{
	this->body = body;
}

const std::string &Message::getBody() const
{
	return body;
}

std::string Message::serializeHeaders() const
{
	std::string result;

	for (const auto &header : headers)
	{
		for (const std::string &value : header.second)
			result += header.first + ": " + value + LINE;
	}
	return result;
}

Request::Request() : method(GET), target("/") {}

Request::Request(method_e method, const std::string &target)
	: method(method), target(target) {}

void Request::setMethod(method_e method)
{
	this->method = method;
}

method_e Request::getMethod() const
{
	return method;
}

void Request::setTarget(const std::string &target)
{
	this->target = target;
}

const std::string &Request::getTarget() const
{
	return target;
}

std::string Request::serialize() const
{
	return methodToString(method) + " " + target + " " + version + LINE
		+ serializeHeaders() + LINE + body;
}

Response::Response() {}

Response::Response(int status)
{
	setStatus(status);
}

void Response::setStatus(int status)
{
	if (reasons.find(status) == reasons.end())
		this->status = 500;
	else
		this->status = status;
}

int Response::getStatus() const
{
	return status;
}

std::string Response::getReason() const
{
	std::map<int, std::string>::const_iterator it = reasons.find(status);

	if (it == reasons.end())
		return reasons.at(500);
	return it->second;
}

std::string Response::serialize() const
{
	return version + " " + std::to_string(status) + " " + getReason() + LINE
		+ serializeHeaders() + LINE + body;
}
