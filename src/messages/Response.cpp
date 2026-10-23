/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:55:07 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/22 13:12:40 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response() : Message("HTTP/1.1"), status_(500) {}

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
