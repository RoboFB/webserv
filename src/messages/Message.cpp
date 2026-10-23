/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 11:23:43 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/22 13:23:55 by rgohrig          ###   ########.fr       */
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
