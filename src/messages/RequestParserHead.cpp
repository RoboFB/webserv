/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserHead.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 21:28:57 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/27 16:20:51 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

#include <algorithm>
#include <cctype>
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

ParseStatus Request::parse_request_head()
{
	const std::size_t head_length = header_length(buffer_);
	if (head_length == std::string::npos)
	{
		if (buffer_.size() <= max_header_size)
			return ParseStatus::INCOMPLETE;
		return ParseStatus::BAD;
	}
	if (head_length > max_header_size)
		return ParseStatus::BAD;

	const std::string block(buffer_.begin(), buffer_.begin() + head_length - 2);
	const std::size_t request_line_end = block.find(LINE);
	if (request_line_end == std::string::npos ||
		!parse_request_line(block.substr(0, request_line_end)) ||
		!parse_headers(block, request_line_end + 2) || !has_required_host())
	{
		return ParseStatus::BAD;
	}
	offset_ = head_length;
	return ParseStatus::COMPLETE;
}
