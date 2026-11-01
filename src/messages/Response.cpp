/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:55:07 by rgohrig           #+#    #+#             */
/*   Updated: 2026/11/01 23:13:28 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Location.hpp"
#include "Methods.hpp"
#include "Resolver.hpp"

#include <cstdint>
#include <fstream>
#include <system_error>
#include <utility>

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

static std::string make_location_target(
	const Location &location,
	const std::filesystem::path &configured_path)
{
	std::string suffix = configured_path.generic_string();

	while (!suffix.empty() && suffix.front() == '/')
		suffix.erase(0, 1);
	if (location.location_path == "/")
		return "/" + suffix;
	return location.location_path + "/" + suffix;
}

static bool read_file(const std::filesystem::path &file_path,
					  std::string &body)
{
	std::error_code error;
	const std::uintmax_t file_size =
		std::filesystem::file_size(file_path, error);

	if (error || file_size > std::string().max_size())
		return false;

	std::ifstream file(file_path, std::ios::binary);
	if (!file)
		return false;

	body.resize(static_cast<std::size_t>(file_size));
	if (!body.empty())
		file.read(body.data(), static_cast<std::streamsize>(body.size()));
	return static_cast<bool>(file);
}

static void error_page(
	Response &response,
	int status,
	const Location &location)
{
	const ErrorPages::const_iterator page =
		location.error_pages.find(status);

	if (page == location.error_pages.end())
		return;

	const ResolvedPath resolved = resolve_resource_path(
		location, make_location_target(location, page->second));

	if (resolved.status != ResolveStatus::OK)
		return;

	std::error_code error;
	const std::filesystem::file_status page_status =
		std::filesystem::status(resolved.path, error);

	if (error || !std::filesystem::is_regular_file(page_status))
		return;

	std::string body;
	if (!read_file(resolved.path, body))
		return;

	response.setHeader("Content-Type", "text/html; charset=utf-8")
		.setBody(std::move(body));
}

void Response::convert_to_not_allowed(const Location &location)
{
	convert_to_error(405, &location);
	headers_.set("Allow", allowed_methods(location.limit_except));
}

void Response::convert_to_error(int status, const Location *location)
{
	set_status(status);
	body_ = std::to_string(status_) + " " + getReason() + "\n";
	headers_.clear();
	headers_.set("Content-Type", "text/plain; charset=utf-8");

	if (location != nullptr)
		error_page(*this, status_, *location);
}

void Response::convert_to_redirect(int status, const std::string &location)
{
	set_status(status);
	headers_.clear();
	headers_.set("Location", location);
	body_.clear();
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
	if (body_offset_ < body_.size())
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

Response &Response::setHeader(const std::string &name,
							  const std::string &value)
{
	headers_.set(name, value);
	return *this;
}
