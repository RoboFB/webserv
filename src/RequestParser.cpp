/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:51:03 by modiepge          #+#    #+#             */
/*   Updated: 2026/09/21 01:54:57 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

static constexpr std::size_t max_header_size = 8192;

static	std::size_t	header_length(const	std::vector<uint8_t>& buffer)
{
	static const	std::string	delimiter = "\r\n\r\n";
	const	std::vector<uint8_t>::const_iterator	found = std::search(buffer.begin(), buffer.end(), delimiter.begin(), delimiter.end());

	if (found == buffer.end())
		return (std::string::npos);
	return (static_cast<std::size_t>(found - buffer.begin()) + delimiter.size());
}

static	std::string lower_case(const std::string& input) {
	std::string string = input;
	std::for_each(string.begin(), string.end(), [](char& c) {c = std::tolower(c);});
	return (string);
}

static std::string trim(const std::string& input) {
	const std::size_t first = input.find_first_not_of(" \t");
    if (first == std::string::npos)
        return "";
    const std::size_t last = input.find_last_not_of(" \t");
    return (input.substr(first, last - first + 1));
}

static bool	is_valid_header_name(const std::string& name){
	static const std::string charset = "!#$%&'*+-.^_`|~";
	if (name.empty())
		return (false);
	for (unsigned char character : name) {
		if (!std::isalnum(character) && charset.find(static_cast<char>(character)) == std::string::npos)
			return (false);
	}
	return (true);
}

RequestParsing request_parsing(const std::vector<uint8_t>& buffer) {
	RequestParsing request;
	const	std::size_t	header = header_length(buffer);
	if (header == std::string::npos) {
		if (buffer.size() > max_header_size)
			return (request);
		request.status = RequestStatus::MSG_INCOMPLETE;
		return request;
	}
	if (header > max_header_size)
		return (request);
	const	std::string header_block(buffer.begin(), buffer.begin() + header - 2);
	std::size_t cursor = 0;
	const std::size_t	first_line = header_block.find(LINE, cursor);
	if (first_line == std::string::npos)
		return (request);
	const std::string	request_line = header_block.substr(cursor, first_line - cursor);
	const std::size_t	first_space = request_line.find(' ');
	const std::size_t	second_space = request_line.find(' ', first_space + 1);
	if (first_space == std::string::npos || second_space == std::string::npos || request_line.find(' ', second_space + 1) != std::string::npos)
		return (request);
	const	Methods method = string_to_methods(request_line.substr(0, first_space));
	if (method == Methods::NONE)
		return (request);
	const	std::string	target = request_line.substr(first_space + 1, second_space - first_space - 1);
	const	std::string	version = request_line.substr(second_space + 1);
	if (method == Methods::NONE || target.empty() || target[0] != '/' || (version != "HTTP/1.0" && version != "HTTP/1.1"))
		return (request);
	request.message.setMethod(method);
	request.message.setTarget(target);
	request.message.setVersion(version);
	cursor = first_line + 2;
	while (cursor < header_block.size()) {
		const std::size_t line_end = header_block.find(LINE, cursor);
		if (line_end == std::string::npos)
			return (request);
		const std::string line = header_block.substr(cursor, line_end - cursor);
		cursor = line_end + 2;
		if (line.empty())
			break;
		const std::size_t	colon = line.find(':');
		if (colon == std::string::npos || colon == 0)
			return (request);
		std::string name = line.substr(0, colon);
		std::string value = line.substr(colon + 1);

		name = lower_case(name);
		value = trim(value);
		if (!is_valid_header_name(name))
			return (request);
		request.message.addHeader(name, value);
	}

	const	std::map<std::string, std::vector<std::string>>& headers = request.message.getHeaders();
	const	std::map<std::string, std::vector<std::string>>::const_iterator host = headers.find("host");
	if (version == "HTTP/1.1" && (host == headers.end() || host->second.size() != 1 || host->second[0].empty()))
		return (request);
	if (request.message.getMethod() == Methods::GET && headers.find("transfer-encoding") != headers.end())
		return (request);
	const std::map<std::string, std::vector<std::string> >::const_iterator length = headers.find("content-length");
	if (length != headers.end() &&
		(length->second.size() != 1 || length->second[0] != "0"))
		return request;
	request.status = RequestStatus::MSG_COMPLETE;
	request.bytes = header;
	return (request);
}
