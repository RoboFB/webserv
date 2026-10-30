/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 11:23:43 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/30 16:23:18 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

Message::Message(const std::string &version) : version_(version) {}

Message::~Message() {}

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

Message &Message::setBody(const std::string &body)
{
	this->body_ = body;
	return *this;
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
			result += prefix + header.first + ": " + value + LINE;
	}
	return result;
}
