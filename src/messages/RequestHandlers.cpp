/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:28 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 23:28:45 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"
#include "Resolver.hpp"
#include "URITarget.hpp"
#include <optional>
#include <fstream>
#include <sstream>
#include <system_error>

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
		if (!is_path_within_root(candidate, canonical_root))
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

	const ResolvedPath resolved_path =
		resolve_resource_path(location, request_path);

	if (resolved_path.status != ResolveStatus::OK)
		return make_error_response(403);

	std::error_code error;
	const std::filesystem::file_status status =
		std::filesystem::status(resolved_path.path, error);
	if (!std::filesystem::exists(status))
	{
		if (error == std::make_error_code(
				std::errc::no_such_file_or_directory))
			return make_error_response(404);

		return make_error_response(403);
	}

	if (error)
		return make_error_response(403);

	std::filesystem::path file_path = resolved_path.path;

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

	const ResolvedPath resolved_path =
		resolve_resource_path(location, request_path);

	if (resolved_path.status != ResolveStatus::OK)
		return make_error_response(403);

	std::error_code error;
	const std::filesystem::file_status existing_status =
		std::filesystem::status(resolved_path.path, error);

	const bool existed = !error && std::filesystem::exists(existing_status);

	if (existed && !std::filesystem::is_regular_file(existing_status))
		return make_error_response(403);

	error.clear();
	const std::filesystem::file_status parent_status =
		std::filesystem::status(resolved_path.path.parent_path(), error);

	if (error || !std::filesystem::is_directory(parent_status))
		return make_error_response(404);

	std::ofstream file(
		resolved_path.path.string().c_str(),
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

	const ResolvedPath resolved_path =
		resolve_resource_path(location, request_path);

	if (resolved_path.status != ResolveStatus::OK)
		return make_error_response(403);

	std::error_code error;
	const std::filesystem::file_status status =
		std::filesystem::status(resolved_path.path, error);

	if (error || !std::filesystem::exists(status))
		return make_error_response(404);

	if (!std::filesystem::is_regular_file(status))
		return make_error_response(403);

	if (!std::filesystem::remove(resolved_path.path, error) || error)
		return make_error_response(500);

	return make_empty_response(204);
}

Response handle_request(const Request& request, const Server& server)
{
	const URITarget target = split_uri_target(request.getTarget());
	const std::string& request_path = target.path;
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
