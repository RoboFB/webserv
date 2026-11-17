/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserBody.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 20:29:59 by modiepge          #+#    #+#             */
/*   Updated: 2026/11/02 00:13:39 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParserBody.hpp"
#include "Syntax.hpp"
#include <algorithm>
#include <charconv>
#include <cctype>
#include <string_view>

/*	Fixed length body
 */
static BodyParsing parse_content_length_body(
	const std::vector<uint8_t> &buffer,
	std::size_t body_start,
	const std::string &length_text,
	std::size_t max_body_size)
{
	std::size_t body_length = 0;
	if (!parse_decimal_size(length_text, body_length))
		return BodyParsing();
	if (body_length > max_body_size)
		return BodyParsing(ParseStatus::TOO_LARGE);
	if (body_start > buffer.size() || body_length > buffer.size() - body_start)
		return BodyParsing(ParseStatus::INCOMPLETE);

	const auto first = buffer.begin() + body_start;
	const std::string body(first, first + body_length);

	return BodyParsing(ParseStatus::COMPLETE, body, body_start + body_length);
}

static std::size_t find_line_end(const std::vector<uint8_t> &buffer,
								 std::size_t cursor)
{
	static const std::string delimiter = LINE;
	if (cursor > buffer.size())
		return std::string::npos;
	const std::vector<uint8_t>::const_iterator found =
		std::search(buffer.begin() + cursor, buffer.end(), delimiter.begin(),
					delimiter.end());
	if (found == buffer.end())
		return std::string::npos;
	return static_cast<std::size_t>(found - buffer.begin());
}

static bool parse_chunk_size(std::string_view line, std::size_t &chunk_size)
{
	const std::size_t extension = line.find(';');
	if (extension != std::string_view::npos)
		line = line.substr(0, extension);
	if (line.empty())
		return false;

	const char *first = line.data();
	const char *last = first + line.size();
	const std::from_chars_result parsed =
		std::from_chars(first, last, chunk_size, 16);
	return parsed.ec == std::errc() && parsed.ptr == last;
}

/*	Chunked body, indeterminate length.
	Incremental: decoded chunks are appended to `body` and `consumed` is
	advanced past each complete chunk, so the next call resumes there
	instead of re-decoding the whole buffer (O(n^2) on large uploads).
 */
static ParseStatus parse_chunked_body(const std::vector<uint8_t> &buffer,
									  std::size_t &consumed, std::string &body,
									  size_t max_body_size)
{
	while (true)
	{
		std::size_t cursor = consumed;
		const std::size_t size_line_end = find_line_end(buffer, cursor);
		if (size_line_end == std::string::npos)
			return ParseStatus::INCOMPLETE;

		const std::string_view size_line(
			reinterpret_cast<const char *>(buffer.data() + cursor),
			size_line_end - cursor);
		std::size_t chunk_size = 0;
		if (!parse_chunk_size(size_line, chunk_size))
			return ParseStatus::BAD;

		cursor = size_line_end + 2;
		if (chunk_size == 0)
		{
			while (true)
			{
				const std::size_t trailer_end = find_line_end(buffer, cursor);
				if (trailer_end == std::string::npos)
					return ParseStatus::INCOMPLETE;
				if (trailer_end == cursor) // empty line ends the trailers
				{
					consumed = trailer_end + 2;
					return ParseStatus::COMPLETE;
				}
				cursor = trailer_end + 2;
			}
		}

		if (body.size() > max_body_size ||
			chunk_size > max_body_size - body.size())
			return ParseStatus::TOO_LARGE;
		if (cursor > buffer.size() || chunk_size > buffer.size() - cursor)
			return ParseStatus::INCOMPLETE;
		if (buffer.size() - cursor - chunk_size < 2)
			return ParseStatus::INCOMPLETE;
		if (buffer[cursor + chunk_size] != '\r' ||
			buffer[cursor + chunk_size + 1] != '\n')
			return ParseStatus::BAD;

		body.append(
			reinterpret_cast<const char *>(buffer.data() + cursor), chunk_size);
		consumed = cursor + chunk_size + 2;
	}
}

/*	Read and fill in body, either chunked or predictable lenght
 */
ParseStatus Request::parse_body(size_t max_body_size)
{
	const HeaderValues *transfer = headers_.find("transfer-encoding");
	const HeaderValues *length = headers_.find("content-length");
	BodyParsing result;

	if (transfer != nullptr && length != nullptr)
		return ParseStatus::BAD;
	if (transfer != nullptr)
	{
		if (transfer->size() != 1 ||
			ascii_lowercase(transfer->at(0)) != "chunked")
			return ParseStatus::BAD;
		// advances offset_ and fills body_ in place, resuming across calls
		return parse_chunked_body(buffer_, offset_, body_, max_body_size);
	}
	else if (length == nullptr)
		result = BodyParsing(ParseStatus::COMPLETE, "", offset_);
	else if (length->size() != 1)
		return ParseStatus::BAD;
	else
		result = parse_content_length_body(
			buffer_, offset_, length->at(0), max_body_size);

	if (result.status == ParseStatus::COMPLETE)
	{
		body_ = std::move(result.body);
		offset_ = result.bytes;
	}
	return result.status;
}
