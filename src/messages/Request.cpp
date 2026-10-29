/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:55:07 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/27 16:03:16 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Syntax.hpp"

Request::Request() : Message(), method_(Methods::GET), target_("/") {}

Request::Request(Methods method, const std::string &target)
	: Message(), method_(method), target_(target)
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

void Request::serialize()
{
	const std::string method_string = methods_to_string(method_) + " " +
									  target_ + " " + version_ + LINE +
									  serializeHeaders() + LINE;

	offset_ = 0;
	buffer_.clear();
	buffer_.reserve(method_string.size() + body_.size());
	buffer_.insert(buffer_.end(), method_string.begin(), method_string.end());
	buffer_.insert(buffer_.end(), body_.begin(), body_.end());
}

bool Request::is_version_valid() const
{
	return version_ == "HTTP/1.0" || version_ == "HTTP/1.1";
}

bool Request::is_method_valid() const
{
	return method_ != Methods::NONE;
}

bool Request::is_target_valid() const
{
	return !target_.empty() && target_[0] == '/';
}

bool Request::parse_request_line(const std::string &line)
{
	const std::size_t first_space = line.find(' ');
	const std::size_t second_space = line.find(' ', first_space + 1);
	if (first_space == std::string::npos || second_space == std::string::npos ||
		line.find(' ', second_space + 1) != std::string::npos)
		return false;

	method_ = string_to_methods(line.substr(0, first_space));
	target_ = line.substr(first_space + 1, second_space - first_space - 1);
	version_ = line.substr(second_space + 1);
	return is_method_valid() && is_target_valid() && is_version_valid();
}

bool Request::parse_headers(const std::string &block, std::size_t cursor)
{
	while (cursor < block.size())
	{
		const std::size_t line_end = block.find(LINE, cursor);

		if (line_end == std::string::npos)
			return false;

		const std::string line = block.substr(cursor, line_end - cursor);

		const std::optional<HeaderField> field = parse_header_field(line);

		if (!field)
			return false;

		addHeader(field->normalized_name, field->value);
		cursor = line_end + 2;
	}
	return true;
}

bool Request::has_required_host() const
{
	if (version_ != "HTTP/1.1")
		return true;
	const auto host = headers_.find("host");
	if (host == headers_.end())
		return false;
	const std::vector<std::string> &values = host->second;
	return values.size() == 1 && !values[0].empty();
}