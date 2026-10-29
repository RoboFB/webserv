/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:28 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/28 16:48:55 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
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

void handle_get(Response &response, const Location &location,
				const std::string &request_path)
{
	const ResolvedPath resolved_path =
		resolve_resource_path(location, request_path);

	if (resolved_path.status != ResolveStatus::OK)
		return response.convert_to_error(403);

	std::error_code error;
	const std::filesystem::file_status status =
		std::filesystem::status(resolved_path.path, error);
	if (!std::filesystem::exists(status))
	{
		if (error == std::make_error_code(std::errc::no_such_file_or_directory))
			return response.convert_to_error(404);

		return response.convert_to_error(403);
	}

	if (error)
		return response.convert_to_error(403);

	std::filesystem::path file_path = resolved_path.path;

	if (std::filesystem::is_directory(status))
	{
		const std::optional<std::filesystem::path> index_file =
			find_index_file(location, file_path);

		if (!index_file)
			return response.convert_to_error(403);

		file_path = *index_file;
	}
	else if (!std::filesystem::is_regular_file(status))
		return response.convert_to_error(403);

	std::ifstream file(file_path, std::ios::binary);
	if (!file)
		return response.convert_to_error(403);

	std::ostringstream contents;
	contents << file.rdbuf();

	if (file.bad())
		return response.convert_to_error(500);

	const std::string body = contents.str();

	response.set_status(200);
	response.addHeader("Content-Type", mime_type_for(file_path))
		.addHeader("Content-Length", std::to_string(body.size()))
		.addHeader("Connection", "close")
		.setBody(body);
	return;
}

void handle_post(const std::string &data_str_body, Response &response,
				 const Location &location, const std::string &request_path)
{
	if (data_str_body.size() > location.client_max_body_size)
		return response.convert_to_error(413);

	const ResolvedPath resolved_path =
		resolve_resource_path(location, request_path);

	if (resolved_path.status != ResolveStatus::OK)
		return response.convert_to_error(403);

	std::error_code error;
	const std::filesystem::file_status existing_status =
		std::filesystem::status(resolved_path.path, error);

	const bool existed = !error && std::filesystem::exists(existing_status);

	if (existed && !std::filesystem::is_regular_file(existing_status))
		return response.convert_to_error(403);

	error.clear();
	const std::filesystem::file_status parent_status =
		std::filesystem::status(resolved_path.path.parent_path(), error);

	if (error || !std::filesystem::is_directory(parent_status))
		return response.convert_to_error(404);

	std::ofstream file(resolved_path.path.string().c_str(),
					   std::ios::binary | std::ios::trunc);

	if (!file)
		return response.convert_to_error(403);

	file.write(data_str_body.data(),
			   static_cast<std::streamsize>(data_str_body.size()));

	if (!file)
		return response.convert_to_error(500);

	return response.set_status(existed ? 204 : 201);
}

void handle_delete(Response &response, const Location &location,
				   const std::string &request_path)
{
	const ResolvedPath resolved_path =
		resolve_resource_path(location, request_path);

	if (resolved_path.status != ResolveStatus::OK)
		return response.convert_to_error(403);

	std::error_code error;
	const std::filesystem::file_status status =
		std::filesystem::status(resolved_path.path, error);

	if (error || !std::filesystem::exists(status))
		return response.convert_to_error(404);

	if (!std::filesystem::is_regular_file(status))
		return response.convert_to_error(403);

	if (!std::filesystem::remove(resolved_path.path, error) || error)
		return response.convert_to_error(500);

	return response.set_status(204);
}

void handle_request(const Request &request, Response &response,
					const Server &server)
{
	const std::string &request_path = request.split_uri_target().path;
	const Location *location = server.find_location(request_path);

	if (location == NULL)
		return response.convert_to_error(404);

	if (!is_method(location->limit_except, request.get_method()))
		return response.convert_to_not_allowed(*location);

	switch (request.get_method())
	{
	case Methods::GET:
		return handle_get(response, *location, request_path);
	case Methods::POST:
		return handle_post(request.getBody(), response, *location,
						   request_path);
	case Methods::DELETE:
		return handle_delete(response, *location, request_path);
	default:
		return response.convert_to_error(501);
	}
}
