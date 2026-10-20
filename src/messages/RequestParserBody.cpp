/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserBody.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 20:29:59 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/19 00:24:09 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParserBody.hpp"
#include "Syntax.hpp"
#include <algorithm>
#include <charconv>
#include <cctype>
#include <limits>
#include <string_view>

static BodyParsing make_incomplete()
{
	BodyParsing result;
	result.status = ParseStatus::INCOMPLETE;
	return result;
}

static BodyParsing make_complete(const std::string& body, std::size_t bytes)
{
	BodyParsing result;
	result.status = ParseStatus::COMPLETE;
	result.body = body;
	result.bytes = bytes;
	return result;
}

/*	Fixed length body
*/
static BodyParsing parse_content_length_body(
	const std::vector<uint8_t>& buffer,
	std::size_t body_start,
	const std::string& length_text)
{
	std::size_t body_length = 0;
	if (!parse_decimal_size(length_text, body_length))
		return BodyParsing();
	if (body_start > buffer.size() || body_length > buffer.size() - body_start)
		return make_incomplete();

	const std::string body(
		reinterpret_cast<const char*>(buffer.data() + body_start), body_length);
	return make_complete(body, body_start + body_length);
}

static std::size_t find_line_end(
	const std::vector<uint8_t>& buffer,
	std::size_t cursor)
{
	static const std::string delimiter = LINE;
	if (cursor > buffer.size())
		return std::string::npos;
	const std::vector<uint8_t>::const_iterator found = std::search(
		buffer.begin() + cursor, buffer.end(),
		delimiter.begin(), delimiter.end());
	if (found == buffer.end())
		return std::string::npos;
	return static_cast<std::size_t>(found - buffer.begin());
}

static bool parse_chunk_size(std::string_view line, std::size_t& chunk_size)
{
	const std::size_t extension = line.find(';');
	if (extension != std::string_view::npos)
		line = line.substr(0, extension);
	if (line.empty())
		return false;

	const char* first = line.data();
	const char* last = first + line.size();
	const std::from_chars_result parsed =
		std::from_chars(first, last, chunk_size, 16);
	return parsed.ec == std::errc() && parsed.ptr == last;
}

/*	Chunked body, indeterminate length
*/
static BodyParsing parse_chunked_body(
	const std::vector<uint8_t>& buffer,
	std::size_t body_start)
{
	BodyParsing result;
	std::size_t cursor = body_start;

	while (true)
	{
		const std::size_t size_line_end = find_line_end(buffer, cursor);
		if (size_line_end == std::string::npos)
			return make_incomplete();

		const std::string_view size_line(
			reinterpret_cast<const char*>(buffer.data() + cursor),
			size_line_end - cursor);
		std::size_t chunk_size = 0;
		if (!parse_chunk_size(size_line, chunk_size))
			return BodyParsing();

		cursor = size_line_end + 2;
		if (chunk_size == 0)
		{
			while (true)
			{
				const std::size_t trailer_end = find_line_end(buffer, cursor);
				if (trailer_end == std::string::npos)
					return make_incomplete();
				if (trailer_end == cursor)
					return make_complete(result.body, trailer_end + 2);
				cursor = trailer_end + 2;
			}
		}

		if (cursor > buffer.size() || chunk_size > buffer.size() - cursor)
			return make_incomplete();
		if (chunk_size > result.body.max_size() - result.body.size())
			return BodyParsing();
		result.body.append(
			reinterpret_cast<const char*>(buffer.data() + cursor), chunk_size);
		cursor += chunk_size;

		if (buffer.size() - cursor < 2)
			return make_incomplete();
		if (buffer[cursor] != '\r' || buffer[cursor + 1] != '\n')
			return BodyParsing();
		cursor += 2;
	}
}

/*	Read and fill in body, either chunked or predictable lenght
*/
BodyParsing parse_body(
	const std::vector<uint8_t>& buffer,
	const Headers& headers,
	std::size_t body_start)
{
	const Headers::const_iterator transfer = headers.find("transfer-encoding");
	const Headers::const_iterator length = headers.find("content-length");
	if (transfer != headers.end() && length != headers.end())
		return BodyParsing();
	if (transfer != headers.end())
	{
		if (transfer->second.size() != 1 ||
			ascii_lowercase(transfer->second[0]) != "chunked")
			return BodyParsing();
		return parse_chunked_body(buffer, body_start);
	}
	if (length == headers.end())
		return make_complete("", body_start);
	if (length->second.size() != 1)
		return BodyParsing();
	return parse_content_length_body(buffer, body_start, length->second[0]);
}
