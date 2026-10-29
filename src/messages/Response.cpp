/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:55:07 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/28 16:41:21 by rgohrig          ###   ########.fr       */
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
	std::string head;
	// if (reasons_.find(status_) != reasons_.end())
	// {
	// 	*this = make_error_response(status_);
	// }
	addHeader("Connection", "close");
	addHeader("Content-Length", std::to_string(body_.size()));

	head = version_ + " " + std::to_string(status_) + " " + getReason() + LINE +
		   serializeHeaders() + LINE;

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
