/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:55:07 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/27 17:00:51 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response() : Message("HTTP/1.1"), status_(500) {}

Response::Response(int status) : Message("HTTP/1.1"), status_(500)
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

void Response::serialize()
{
	const std::string head = version_ + " " + std::to_string(status_) + " " +
							 getReason() + LINE + serializeHeaders() + LINE;

	offset_ = 0;
	buffer_.clear();
	buffer_.reserve(head.size() + body_.size());
	buffer_.insert(buffer_.end(), head.begin(), head.end());
	buffer_.insert(buffer_.end(), body_.begin(), body_.end());
}

Response &Response::addHeader(const std::string &name, const std::string &value)
{
	Message::addHeader(name, value);
	return *this;
}
