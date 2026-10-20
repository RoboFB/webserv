/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:28 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/20 22:16:52 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"
#include "Resolver.hpp"
#include <optional>
#include <fstream>
#include <sstream>
#include <system_error>

static std::optional<std::filesystem::path>
find_index_file(const Location &location,
				const std::filesystem::path &directory)
{
	std::error_code error;
	const std::filesystem::path canonical_root =
		std::filesystem::weakly_canonical(location.root_path, error);
	if (error)
		return (std::nullopt);
	for (const std::filesystem::path &index : location.indexs_paths)
	{
		if (index.is_absolute())
			continue;
		const std::filesystem::path candidate =
			std::filesystem::weakly_canonical(directory / index, error);
		if (error)
		{
			error.clear();
			continue;
		}
		if (!is_path_within_root(candidate, canonical_root))
			continue;
		const std::filesystem::file_status status =
			std::filesystem::status(candidate, error);
		if (error)
		{
			error.clear();
			continue;
		}
		if (std::filesystem::is_regular_file(status))
			return (candidate);
	}
	return (std::nullopt);
}

// temporary until I can get a better list
static std::string mime_type_for(const std::filesystem::path &path)
{
	const std::string extension = path.extension().string();

	if (extension == ".html" || extension == ".htm")
		return ("text/html; charset=utf-8");
	else if (extension == ".css")
		return ("text/css; charset=utf-8");
	else if (extension == ".js")
		return ("text/javascript; charset=utf-8");
	else if (extension == ".txt")
		return ("text/plain; charset=utf-8");
	else if (extension == ".json")
		return ("application/json");
	else if (extension == ".svg")
		return ("image/svg+xml");
	else if (extension == ".png")
		return ("image/png");
	else if (extension == ".jpg" || extension == ".jpeg")
		return ("image/jpeg");
	else
		return "application/octet-stream";
}

Response make_error_response(int status)
{
	const std::string body =
		std::to_string(status) + " " + Response(status).getReason() + "\n";

	Response response(status);
	response.setVersion("HTTP/1.1")
		.addHeader("Content-Type", "text/plain; charset=utf-8")
		.addHeader("Content-Length", std::to_string(body.size()))
		.addHeader("Connection", "close")
		.setBody(body);
	return (response);
}

static Response make_empty_response(int status)
{
	Response response(status);

	response.setVersion("HTTP/1.1")
		.addHeader("Content-Length", "0")
		.addHeader("Connection", "close");
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

static Response make_method_not_allowed_response(const Location &location)
{
	return make_empty_response(405).addHeader(
		"Allow", allowed_methods(location.limit_except));
}

Response Request::handle_get(const Location &location,
							 const std::string &request_path) const
{
	const ResolvedPath resolved_path =
		resolve_resource_path(location, request_path);

	if (resolved_path.status != ResolveStatus::OK)
		return make_error_response(403);

	std::error_code error;
	const std::filesystem::file_status status =
		std::filesystem::status(resolved_path.path, error);
	if (!std::filesystem::exists(status))
	{
		if (error == std::make_error_code(std::errc::no_such_file_or_directory))
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
	response.setVersion("HTTP/1.1")
		.addHeader("Content-Type", mime_type_for(file_path))
		.addHeader("Content-Length", std::to_string(body.size()))
		.addHeader("Connection", "close")
		.setBody(body);
	return response;
}

Response Request::handle_post(const Location &location,
							  const std::string &request_path) const
{
	if (getBody().size() > location.client_max_body_size)
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

	std::ofstream file(resolved_path.path.string().c_str(),
					   std::ios::binary | std::ios::trunc);

	if (!file)
		return make_error_response(403);

	file.write(getBody().data(),
			   static_cast<std::streamsize>(getBody().size()));

	if (!file)
		return make_error_response(500);

	return make_empty_response(existed ? 204 : 201);
}

Response Request::handle_delete(const Location &location,
								const std::string &request_path) const
{
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

Response Request::handle_request(const Server &server) const
{
	const std::string &request_path = split_uri_target().path;
	const Location *location = server.find_location(request_path);

	if (location == NULL)
		return make_error_response(404);

	if (!is_method(location->limit_except, method_))
		return make_method_not_allowed_response(*location);

	switch (method_)
	{
	case Methods::GET:
		return handle_get(*location, request_path);
	case Methods::POST:
		return handle_post(*location, request_path);
	case Methods::DELETE:
		return handle_delete(*location, request_path);
	default:
		return make_error_response(501);
	}
}
