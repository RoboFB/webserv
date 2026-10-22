/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserHead.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 21:28:57 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/22 13:39:03 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParserHead.hpp"

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

	Request request;
	const std::string block(buffer.begin(), buffer.begin() + head_length - 2);
	const std::size_t request_line_end = block.find(LINE);
	if (request_line_end == std::string::npos ||
		!request.parse_request_line(block.substr(0, request_line_end)) ||
		!request.parse_headers(block, request_line_end + 2) ||
		!request.has_required_host())
	{
		result.request = request;
		return result;
	}
	result.request = request;
	result.body_start = head_length;
	result.status = ParseStatus::COMPLETE;
	return result;
}
