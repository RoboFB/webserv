/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:28 by modiepge          #+#    #+#             */
/*   Updated: 2026/11/02 00:13:58 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Resolver.hpp"

#include <optional>
#include <fstream>
#include <cstdint>
#include <utility>
#include <system_error>
#include <utility>

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

static std::string add_trailing_slash(const std::string &request_target)
{
	const std::size_t query_start = request_target.find('?');

	if (query_start == std::string::npos)
		return request_target + "/";

	return request_target.substr(0, query_start) + "/" +
		   request_target.substr(query_start);
}

static std::string html_escape(const std::string &text)
{
	std::string escaped;

	for (char character : text)
	{
		if (character == '&')
			escaped += "&amp;";
		else if (character == '<')
			escaped += "&lt;";
		else if (character == '>')
			escaped += "&gt;";
		else if (character == '"')
			escaped += "&quot;";
		else
			escaped += character;
	}
	return escaped;
}

static bool autoindex(const Location &location,
						   const std::filesystem::path &directory,
						   const std::string &request_path,
						   std::string &body)
{
	std::error_code error;
	std::filesystem::directory_iterator iterator(directory, error);

	if (error)
		return false;

	body += "<!doctype html>\n";
	body += "<html><head><meta charset=\"utf-8\">";
	body += "<title>Index of ";
	body += html_escape(request_path);
	body += "</title></head><body>\n";
	body += "<h1>Index of ";
	body += html_escape(request_path);
	body += "</h1><ul>\n";

	const bool is_location_root =
		request_path == "/" ||
		request_path == location.location_path ||
		request_path == location.location_path + "/";

	if (!is_location_root)
		body += "<li><a href=\"../\">../</a></li>\n";

	while (iterator != std::filesystem::directory_iterator())
	{
		const std::string name = iterator->path().filename().string();
		const bool is_directory = iterator->is_directory(error);

		if (error)
			return false;

		body += "<li><a href=\"";
		body += percent_encode(name);
		if (is_directory)
			body += "/";
		body += "\">";
		body += html_escape(name);
		if (is_directory)
			body += "/";
		body += "</a></li>\n";

		iterator.increment(error);
		if (error)
			return false;
	}

	body += "</ul></body></html>\n";
	return true;
}

void handle_get(Response &response, const Location &location,
				const std::string &request_path,
				const std::string &request_target)
{
	const ResolvedPath resolved_path =
		resolve_resource_path(location, request_path);

	if (resolved_path.status != ResolveStatus::OK)
	{
		response.convert_to_error(403, &location);
		return;
	}

	std::error_code error;
	const std::filesystem::file_status status =
		std::filesystem::status(resolved_path.path, error);

	if (!std::filesystem::exists(status))
	{
		if (error == std::make_error_code(
				std::errc::no_such_file_or_directory))
			response.convert_to_error(404, &location);
		else
			response.convert_to_error(403, &location);
		return;
	}

	if (error)
	{
		response.convert_to_error(403, &location);
		return;
	}

	std::filesystem::path file_path = resolved_path.path;

	if (std::filesystem::is_directory(status))
	{
		if (request_path.empty() || request_path.back() != '/')
		{
			response.convert_to_redirect(
				301, add_trailing_slash(request_target));
			return;
		}

		const std::optional<std::filesystem::path> index_file =
			find_index_file(location, file_path);

		if (index_file)
			file_path = *index_file;
		else
		{
			if (!location.autoindex)
			{
				response.convert_to_error(403, &location);
				return;
			}

			std::string body;

			if (!autoindex(location, file_path, request_path, body))
			{
				response.convert_to_error(403, &location);
				return;
			}

			response.set_status(200);
			response.addHeader("Content-Type", "text/html; charset=utf-8")
				.setBody(std::move(body));
			return;
		}
	}
	else if (!std::filesystem::is_regular_file(status))
	{
		response.convert_to_error(403, &location);
		return;
	}

	error.clear();
	const std::uintmax_t file_size =
		std::filesystem::file_size(file_path, error);

	if (error || file_size > std::string().max_size())
	{
		response.convert_to_error(500, &location);
		return;
	}

	std::ifstream file(file_path, std::ios::binary);

	if (!file)
	{
		response.convert_to_error(403, &location);
		return;
	}

	std::string body(static_cast<std::size_t>(file_size), '\0');

	if (!body.empty())
		file.read(body.data(), static_cast<std::streamsize>(body.size()));

	if (!file)
	{
		response.convert_to_error(500, &location);
		return;
	}

	response.set_status(200);
	response.addHeader("Content-Type", mime_type_for(file_path))
		.setBody(std::move(body));
}

void handle_post(const std::string &data_str_body, Response &response,
				 const Location &location, const std::string &request_path)
{
	if (data_str_body.size() > location.client_max_body_size)
	{
		response.convert_to_error(413, &location);
		return;
	}

	const ResolvedPath resolved_path =
		resolve_resource_path(location, request_path);

	if (resolved_path.status != ResolveStatus::OK)
	{
		response.convert_to_error(403, &location);
		return;
	}

	std::error_code error;
	const std::filesystem::file_status existing_status =
		std::filesystem::status(resolved_path.path, error);

	const bool existed = !error && std::filesystem::exists(existing_status);

	if (existed && !std::filesystem::is_regular_file(existing_status))
	{
		response.convert_to_error(403, &location);
		return;
	}

	error.clear();
	const std::filesystem::file_status parent_status =
		std::filesystem::status(resolved_path.path.parent_path(), error);

	if (error || !std::filesystem::is_directory(parent_status))
	{
		response.convert_to_error(404, &location);
		return;
	}

	std::ofstream file(resolved_path.path, std::ios::binary | std::ios::trunc);

	if (!file)
	{
		response.convert_to_error(403, &location);
		return;
	}

	file << data_str_body;

	if (!file)
	{
		response.convert_to_error(500, &location);
		return;
	}

	response.set_status(existed ? 204 : 201);
}

void handle_delete(Response &response, const Location &location,
				   const std::string &request_path)
{
	const ResolvedPath resolved_path =
		resolve_resource_path(location, request_path);

	if (resolved_path.status != ResolveStatus::OK)
	{
		response.convert_to_error(403, &location);
		return;
	}

	std::error_code error;
	const std::filesystem::file_status status =
		std::filesystem::status(resolved_path.path, error);

	if (error || !std::filesystem::exists(status))
	{
		response.convert_to_error(404, &location);
		return;
	}

	if (!std::filesystem::is_regular_file(status))
	{
		response.convert_to_error(403, &location);
		return;
	}

	if (!std::filesystem::remove(resolved_path.path, error) || error)
	{
		response.convert_to_error(500, &location);
		return;
	}

	response.set_status(204);
}

void handle_request(const Request &request, Response &response,
					const Server &server)
{
	const URITarget target = request.split_uri_target();
	const Location *location = server.find_location(target.path);

	if (location == NULL)
	{
		response.convert_to_error(404, location);
		return;
	}

	if (location->redirect_page)
	{
		response.convert_to_redirect(
			location->redirect_page->first,
			location->redirect_page->second);
		return;
	}

	if (!is_method(location->limit_except, request.get_method()))
	{
		response.convert_to_not_allowed(*location);
		return;
	}

	switch (request.get_method())
	{
	case Methods::GET:
		handle_get(response, *location, target.path, request.getTarget());
		return;
	case Methods::POST:
		handle_post(request.getBody(), response, *location, target.path);
		return;
	case Methods::DELETE:
		handle_delete(response, *location, target.path);
		return;
	default:
		response.convert_to_error(501, location);
		return;
	}
}
