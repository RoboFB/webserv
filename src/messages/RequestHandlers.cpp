/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:28 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 22:44:12 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"
#include <optional>
#include <fstream>
#include <sstream>
#include <system_error>

static std::string path_without_query(const Request& request) {
	const std::string target = request.getTarget().string();
	const std::size_t query_start = target.find('?');
	if (query_start == std::string::npos)
		return (target);
	return (target.substr(0, query_start));
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

static std::optional<std::filesystem::path> find_index_file(const Location& location, const std::filesystem::path& directory) {
	std::error_code error;
	const std::filesystem::path canonical_root = std::filesystem::weakly_canonical(location.root_path, error);
	if (error)
		return (std::nullopt);
	for (const std::filesystem::path& index : location.indexs_paths) {
		if (index.is_absolute())
			continue;
		const std::filesystem::path candidate =
			std::filesystem::weakly_canonical(directory / index, error);
		if (error) {
			error.clear();
			continue;
		}
		if (!is_within_root(candidate, canonical_root))
			continue;
		const std::filesystem::file_status status =
			std::filesystem::status(candidate, error);
		if (error) {
			error.clear();
			continue;
		}
		if (std::filesystem::is_regular_file(status))
			return (candidate);
	}
	return (std::nullopt);
}

//temporary until I can get a better list
static std::string mime_type_for(const std::filesystem::path& path) {
	const std::string extension = path.extension().string();

	if (extension == ".html" || extension == ".htm")
		return ("text/html; charset=utf-8");
	if (extension == ".css")
        return ("text/css; charset=utf-8");
    if (extension == ".js")
        return ("text/javascript; charset=utf-8");
    if (extension == ".txt")
        return ("text/plain; charset=utf-8");
    if (extension == ".json")
        return ("application/json");
    if (extension == ".svg")
        return ("image/svg+xml");
    if (extension == ".png")
        return ("image/png");
    if (extension == ".jpg" || extension == ".jpeg")
        return ("image/jpeg");

    return "application/octet-stream";
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

static Response make_empty_response(int status)
{
	Response response(status);

	response.setVersion("HTTP/1.1");
	response.addHeader("Content-Length", "0");
	response.addHeader("Connection", "close");
	return response;
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

static Response make_method_not_allowed_response(const Location& location)
{
	Response response = make_empty_response(405);

	response.addHeader("Allow", allowed_methods(location.limit_except));
	return response;
}

Response handle_get(
	const Request& request,
	const Location& location,
	const std::string& request_path)
{
	(void)request;

	const std::optional<std::filesystem::path> resolved_path =
		resolve_path(location, request_path);

	if (!resolved_path)
		return make_error_response(403);

	std::error_code error;
	const std::filesystem::file_status status =
		std::filesystem::status(*resolved_path, error);
	if (!std::filesystem::exists(status))
	{
		if (error == std::make_error_code(
				std::errc::no_such_file_or_directory))
			return make_error_response(404);

		return make_error_response(403);
	}

	if (error)
		return make_error_response(403);

	std::filesystem::path file_path = *resolved_path;

	if (std::filesystem::is_directory(status))
	{
		const std::optional<std::filesystem::path> index_file =
			find_index_file(location, file_path);

		if (!index_file)
			return make_error_response(403);

		file_path = *index_file;
	}
	else if (!std::filesystem::is_regular_file(status))
		return make_error_response(403);

	std::ifstream file(file_path.string().c_str(), std::ios::binary);
	if (!file)
		return make_error_response(403);

	std::ostringstream contents;
	contents << file.rdbuf();

	if (file.bad())
		return make_error_response(500);

	const std::string body = contents.str();

	Response response(200);
	response.setVersion("HTTP/1.1");
	response.addHeader("Content-Type", mime_type_for(file_path));
	response.addHeader("Content-Length", std::to_string(body.size()));
	response.addHeader("Connection", "close");
	response.setBody(body);
	return response;
}

Response handle_post(
	const Request& request,
	const Location& location,
	const std::string& request_path)
{
	if (request.getBody().size() > location.client_max_body_size)
		return make_error_response(413);

	const std::optional<std::filesystem::path> resolved_path =
		resolve_path(location, request_path);

	if (!resolved_path)
		return make_error_response(403);

	std::error_code error;
	const std::filesystem::file_status existing_status =
		std::filesystem::status(*resolved_path, error);

	const bool existed = !error && std::filesystem::exists(existing_status);

	if (existed && !std::filesystem::is_regular_file(existing_status))
		return make_error_response(403);

	error.clear();
	const std::filesystem::file_status parent_status =
		std::filesystem::status(resolved_path->parent_path(), error);

	if (error || !std::filesystem::is_directory(parent_status))
		return make_error_response(404);

	std::ofstream file(
		resolved_path->string().c_str(),
		std::ios::binary | std::ios::trunc);

	if (!file)
		return make_error_response(403);

	file.write(
		request.getBody().data(),
		static_cast<std::streamsize>(request.getBody().size()));

	if (!file)
		return make_error_response(500);

	return make_empty_response(existed ? 204 : 201);
}

Response handle_delete(
	const Request& request,
	const Location& location,
	const std::string& request_path)
{
	(void)request;

	const std::optional<std::filesystem::path> resolved_path =
		resolve_path(location, request_path);

	if (!resolved_path)
		return make_error_response(403);

	std::error_code error;
	const std::filesystem::file_status status =
		std::filesystem::status(*resolved_path, error);

	if (error || !std::filesystem::exists(status))
		return make_error_response(404);

	if (!std::filesystem::is_regular_file(status))
		return make_error_response(403);

	if (!std::filesystem::remove(*resolved_path, error) || error)
		return make_error_response(500);

	return make_empty_response(204);
}

Response handle_request(const Request& request, const Server& server)
{
	const std::string request_path = path_without_query(request);
	const Location* location = server.find_location(request_path);

	if (location == NULL)
		return make_error_response(404);

	if (!is_method(location->limit_except, request.getMethod()))
		return make_method_not_allowed_response(*location);

	switch (request.getMethod())
	{
		case Methods::GET:
			return handle_get(request, *location, request_path);

		case Methods::POST:
			return handle_post(request, *location, request_path);

		case Methods::DELETE:
			return handle_delete(request, *location, request_path);

		default:
			return make_error_response(501);
	}
}
