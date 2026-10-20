/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserHead.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 21:28:57 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/20 21:09:30 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParserHead.hpp"

#include "Syntax.hpp"
#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <vector>

constexpr std::size_t max_header_size = 8192;

static std::size_t header_length(const std::vector<uint8_t> &buffer)
{
	static const std::string delimiter = "\r\n\r\n";
	const std::vector<uint8_t>::const_iterator found = std::search(
		buffer.begin(), buffer.end(), delimiter.begin(), delimiter.end());
	if (found == buffer.end())
		return std::string::npos;
	return (found - buffer.begin()) + delimiter.size();
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

HeaderParsing parse_request_head(const std::vector<uint8_t> &buffer)
{
	HeaderParsing result;
	const std::size_t head_length = header_length(buffer);
	if (head_length == std::string::npos)
	{
		if (buffer.size() <= max_header_size)
			result.status = ParseStatus::INCOMPLETE;
		return result;
	}
	if (head_length > max_header_size)
		return result;

	const std::string block(buffer.begin(), buffer.begin() + head_length - 2);
	const std::size_t request_line_end = block.find(LINE);
	if (request_line_end == std::string::npos ||
		!result.request.parse_request_line(block.substr(0, request_line_end)) ||
		!result.request.parse_headers(block, request_line_end + 2) ||
		!result.request.has_required_host())
		return result;

	result.body_start = head_length;
	result.status = ParseStatus::COMPLETE;
	return result;
}
