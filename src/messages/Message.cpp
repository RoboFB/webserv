/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 11:23:43 by modiepge          #+#    #+#             */
/*   Updated: 2026/11/01 21:32:05 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include <utility>

Message::Message(const std::string &version) : version_(version) {}

Message::~Message() {}

Message &Message::addHeader(const std::string &name, const std::string &value)
{
	headers_.add(name, value);
	return *this;
}

Message &Message::setBody(std::string body)
{
	body_ = std::move(body);
	return *this;
}

const std::string &Message::getBody() const
{
	return body_;
}

std::string Message::serializeHeaders() const
{
	std::string result;

	headers_.serialize(result);
	return result;
}