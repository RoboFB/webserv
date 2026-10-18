/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserHead.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 21:28:57 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 21:40:35 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParserHead.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <vector>

constexpr std::size_t max_header_size = 8192;

static std::size_t header_length(const std::vector<uint8_t>& buffer)
{
	static const std::string delimiter = "\r\n\r\n";
	const std::vector<uint8_t>::const_iterator found = std::search(
		buffer.begin(), buffer.end(), delimiter.begin(), delimiter.end());
	if (found == buffer.end())
		return std::string::npos;
	return static_cast<std::size_t>(found - buffer.begin()) + delimiter.size();
}

static std::string lower_case(const std::string& input)
{
	std::string result = input;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char character) { return std::tolower(character); });
	return result;
}

static std::string trim(const std::string& input)
{
	const std::size_t first = input.find_first_not_of(" \t");
	if (first == std::string::npos)
		return "";
	const std::size_t last = input.find_last_not_of(" \t");
	return input.substr(first, last - first + 1);
}

static bool is_valid_header_name(const std::string& name)
{
	static const std::string allowed = "!#$%&'*+-.^_`|~";
	if (name.empty())
		return false;
	for (std::string::const_iterator it = name.begin(); it != name.end(); ++it)
	{
		const unsigned char character = static_cast<unsigned char>(*it);
		if (!std::isalnum(character) && allowed.find(*it) == std::string::npos)
			return false;
	}
	return true;
}

static bool parse_request_line(const std::string& line, Request& request)
{
	const std::size_t first_space = line.find(' ');
	const std::size_t second_space = line.find(' ', first_space + 1);
	if (first_space == std::string::npos || second_space == std::string::npos ||
		line.find(' ', second_space + 1) != std::string::npos)
		return false;

	const Methods method = string_to_methods(line.substr(0, first_space));
	const std::string target = line.substr(
		first_space + 1, second_space - first_space - 1);
	const std::string version = line.substr(second_space + 1);

	if (method == Methods::NONE || target.empty() || target[0] != '/' ||
		(version != "HTTP/1.0" && version != "HTTP/1.1"))
		return false;

	request.setMethod(method);
	request.setTarget(target);
	request.setVersion(version);
	return true;
}

static bool parse_headers(const std::string& block, std::size_t cursor, Request& request)
{
	while (cursor < block.size())
	{
		const std::size_t line_end = block.find(LINE, cursor);
		if (line_end == std::string::npos)
			return false;
		const std::string line = block.substr(cursor, line_end - cursor);
		const std::size_t colon = line.find(':');
		if (colon == std::string::npos || colon == 0)
			return false;
		const std::string name = lower_case(line.substr(0, colon));
		if (!is_valid_header_name(name))
			return false;
		request.addHeader(name, trim(line.substr(colon + 1)));
		cursor = line_end + 2;
	}
	return true;
}

static bool has_required_host(const Request& request)
{
	if (request.getVersion() != "HTTP/1.1")
		return true;
	const std::map<std::string, std::vector<std::string> >& headers =
		request.getHeaders();
	const std::map<std::string, std::vector<std::string> >::const_iterator host =
		headers.find("host");
	return host != headers.end() && host->second.size() == 1 &&
		!host->second[0].empty();
}

HeaderParsing parse_request_head(const std::vector<uint8_t>& buffer)
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
		!parse_request_line(block.substr(0, request_line_end), result.request) ||
		!parse_headers(block, request_line_end + 2, result.request) ||
		!has_required_host(result.request))
		return result;

	result.body_start = head_length;
	result.status = ParseStatus::COMPLETE;
	return result;
}
