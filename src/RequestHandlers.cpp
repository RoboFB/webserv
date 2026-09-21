/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:28 by modiepge          #+#    #+#             */
/*   Updated: 2026/09/21 02:51:22 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"
#include <optional>
#include <system_error>

static std::string path_without_query(const Request& request) {
	const std::string target = request.getTarget().string();
	const std::size_t query_start = target.find('?');
	if (query_start == std::string::npos)
		return (target);
	return (target.substr(0, query_start));
}

static int hex_value(char character) {
	if (character >= '0' && character <= '9')
		return character - '0';
	if (character >= 'a' && character <= 'f')
		return character - 'a' + 10;
	if (character >= 'A' && character <= 'F')
		return character - 'A' + 10;
	return -1;
}

static bool percent_decode(const std::string& input, std::string& output) {
	output.clear();
	for (std::size_t index = 0; index < input.size(); ++index) {
		if (input[index] != '%') {
			output += input[index];
			continue;
		}
		if (index + 2 >= input.size())
			return (false);
		const int high = hex_value(input[index + 1]);
		const int low = hex_value(input[index + 2]);
		if (high < 0 || low < 0)
			return (false);
		const char decoded = static_cast<char>(high * 16 + low);
		if (decoded == '\0')
			return (false);
		output += decoded;
		index += 2;
	}
	return (true);
}

static bool is_within_root(const std::filesystem::path& candidate, const std::filesystem::path& root) {
	std::filesystem::path::const_iterator root_part = root.begin();
	std::filesystem::path::const_iterator candidate_part = candidate.begin();
	while (root_part != root.end()) {
		if (candidate_part == candidate.end() || *candidate_part != *root_part)
			return (false);
		++root_part;
		++candidate_part;
	}
	return (true);
}

static std::optional<std::filesystem::path> resolve_path(const Location& location, const std::string& request_path){
	std::string suffix;
	if (location.location_path =="/")
		suffix = request_path.substr(1);
	else
		suffix = request_path.substr(location.location_path.size());
	while (!suffix.empty() && suffix[0] == '/')
		suffix.erase(0, 1);
	std::string decoded_suffix;
	if (!percent_decode(suffix, decoded_suffix))
		return (std::nullopt);
	std::filesystem::path relative_path(decoded_suffix);
	if (relative_path.is_absolute())
		return (std::nullopt);
	for (const std::filesystem::path& part : relative_path) {
		if (part == "..")
			return (std::nullopt);
	}
	std::error_code error;
	const std::filesystem::path canonical_root = std::filesystem::weakly_canonical(location.root_path, error);
	if (error)
		return (std::nullopt);
	const std::filesystem::path canonical_candidate = std::filesystem::weakly_canonical(canonical_root / relative_path, error);
	if (error || !is_within_root(canonical_candidate, canonical_root))
		return (std::nullopt);
	return (canonical_candidate);
}

Response make_error_response(int status) {
	const	std::string body = std::to_string(status) + " " + Response(status).getReason() + "\n";
	Response response(status);

	response.setVersion("HTTP/1.1");
	response.addHeader("Content-Type", "text/plain; charset=utf-8");
	response.addHeader("Content-Length", std::to_string(body.size()));
	response.addHeader("Connection", "close");
	response.setBody(body);
	return (response);
}

Response	handle_get(const Request& request, const Server& server) {
	const std::string path = path_without_query(request);
	const Location* location = server.find_location(path);
	if (location == nullptr)
		return (make_error_response(404));
	const std::optional<std::filesystem::path> resolved_path = resolve_path(*location, path);
	if(!resolved_path)
		return (make_error_response(403));

	const std::string body = "Resolved path: " + resolved_path->string() + "\n";
	// const std::string file_contents = read_file();

	Response response(200);
	response.setVersion("HTTP/1.1");
	response.addHeader("Content-Type", "text/plain; charset=utf-8");
	response.addHeader("Content-Length", std::to_string(body.size()));
	response.addHeader("Connection", "close");
	response.setBody(body);
	return response;
}

Response	handle_request(const Request& request, const Server& server) {
	if (request.getMethod() == Methods::GET)
		return (handle_get(request, server));

	Response response(501);
	response.setVersion("HTTPS/1.1");
	response.addHeader("Content-Length", "0");
	response.addHeader("Connection", "close");
	return response;
}
