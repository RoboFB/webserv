/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:28 by modiepge          #+#    #+#             */
/*   Updated: 2026/11/02 00:39:06 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Resolver.hpp"
#include "Syntax.hpp"

#include <optional>
#include <fstream>
#include <cstdint>
#include <utility>
#include <system_error>
#include <utility>

static std::optional<std::string> find_parameter(
	const std::string &value,
	const std::string &wanted_name)
{
	std::size_t cursor = 0;

	while (cursor < value.size())
	{
		const std::size_t end = value.find(';', cursor);
		const std::string parameter = trim_whitespace(
			value.substr(cursor, end - cursor));
		const std::size_t equal = parameter.find('=');

		if (equal != std::string::npos)
		{
			const std::string name = ascii_lowercase(
				trim_whitespace(parameter.substr(0, equal)));

			if (name == wanted_name)
			{
				std::string result = trim_whitespace(
					parameter.substr(equal + 1));

				if (result.size() >= 2 &&
					result.front() == '"' &&
					result.back() == '"')
				{
					result = result.substr(1, result.size() - 2);
				}

				if (!result.empty())
					return result;
				return std::nullopt;
			}
		}

		if (end == std::string::npos)
			break;
		cursor = end + 1;
	}

	return std::nullopt;
}

static std::optional<std::string> multipart_boundary(
	const std::string &content_type)
{
	const std::size_t separator = content_type.find(';');
	const std::string media_type = ascii_lowercase(
		trim_whitespace(content_type.substr(0, separator)));

	if (media_type != "multipart/form-data")
		return std::nullopt;

	if (separator == std::string::npos)
		return std::nullopt;

	return find_parameter(
		content_type.substr(separator + 1), "boundary");
}

static std::optional<std::string> multipart_filename(
	const std::string &header_block)
{
	std::size_t cursor = 0;

	while (cursor < header_block.size())
	{
		const std::size_t line_end = header_block.find(LINE, cursor);

		if (line_end == std::string::npos)
			return std::nullopt;

		const std::optional<HeaderField> field =
			parse_header_field(
				header_block.substr(cursor, line_end - cursor));

		if (!field)
			return std::nullopt;

		if (field->normalized_name == "content-disposition")
			return find_parameter(field->value, "filename");

		cursor = line_end + 2;
	}

	return std::nullopt;
}

static bool parse_multipart_first_file(
	const std::string &content_type,
	const std::string &body,
	std::string &filename,
	std::size_t &file_offset,
	std::size_t &file_size)
{
	const std::optional<std::string> boundary =
		multipart_boundary(content_type);

	if (!boundary)
		return false;

	const std::string marker = "--" + *boundary;
	std::size_t cursor = 0;

	while (true)
	{
		if (body.compare(cursor, marker.size(), marker) != 0)
			return false;

		cursor += marker.size();

		if (body.compare(cursor, 2, "--") == 0)
			return false;

		if (body.compare(cursor, 2, LINE) != 0)
			return false;

		cursor += 2;

		const std::size_t header_end =
			body.find(LINE LINE, cursor);

		if (header_end == std::string::npos)
			return false;

		const std::optional<std::string> part_filename =
			multipart_filename(
				body.substr(cursor, header_end - cursor));

		const std::size_t data_start = header_end + 4;
		const std::size_t next_boundary =
			body.find(LINE + marker, data_start);

		if (next_boundary == std::string::npos)
			return false;

		if (part_filename)
		{
			filename = *part_filename;
			file_offset = data_start;
			file_size = next_boundary - data_start;
			return true;
		}

		cursor = next_boundary + 2;
	}
}

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

static bool is_safe_upload_filename(const std::string &filename)
{
	return !filename.empty() &&
		   filename != "." &&
		   filename != ".." &&
		   filename.find('/') == std::string::npos &&
		   filename.find('\\') == std::string::npos &&
		   filename.find('\0') == std::string::npos;
}

static std::optional<std::filesystem::path> upload_destination(
	const Location &location,
	const std::string &filename)
{
	if (!location.upload_store || !is_safe_upload_filename(filename))
		return std::nullopt;

	std::filesystem::path store = *location.upload_store;

	if (store.is_absolute())
		store = store.relative_path();

	std::error_code error;
	const std::filesystem::path root =
		std::filesystem::weakly_canonical(location.root_path, error);

	if (error)
		return std::nullopt;

	const std::filesystem::path canonical_store =
		std::filesystem::weakly_canonical(root / store, error);

	if (error || !is_path_within_root(canonical_store, root))
		return std::nullopt;

	const std::filesystem::file_status store_status =
		std::filesystem::status(canonical_store, error);

	if (error || !std::filesystem::is_directory(store_status))
		return std::nullopt;

	const std::filesystem::path destination =
		std::filesystem::weakly_canonical(
			canonical_store / filename, error);

	if (error || !is_path_within_root(destination, canonical_store))
		return std::nullopt;

	return destination;
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

void handle_post(const Request &request, Response &response,
				 const Location &location)
{
	const std::string &body = request.getBody();

	if (body.size() > location.client_max_body_size)
	{
		response.convert_to_error(413, &location);
		return;
	}

	const HeaderValues *content_type =
		request.get_headers().find("content-type");

	if (content_type == nullptr || content_type->size() != 1)
	{
		response.convert_to_error(400, &location);
		return;
	}

	std::string filename;
	std::size_t file_offset = 0;
	std::size_t file_size = 0;

	if (!parse_multipart_first_file(
			content_type->at(0), body, filename,
			file_offset, file_size))
	{
		response.convert_to_error(400, &location);
		return;
	}

	if (file_offset > body.size() ||
		file_size > body.size() - file_offset)
	{
		response.convert_to_error(400, &location);
		return;
	}

	const std::optional<std::filesystem::path> destination =
		upload_destination(location, filename);

	if (!destination)
	{
		response.convert_to_error(403, &location);
		return;
	}

	std::error_code error;
	const bool existed =
		std::filesystem::exists(*destination, error);

	if (error)
	{
		response.convert_to_error(500, &location);
		return;
	}

	std::ofstream file(*destination, std::ios::binary | std::ios::trunc);

	if (!file)
	{
		response.convert_to_error(403, &location);
		return;
	}

	if (file_size != 0)
	{
		file.write(body.data() + file_offset,
				   static_cast<std::streamsize>(file_size));
	}

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
		handle_post(request, response, *location);
		return;
	case Methods::DELETE:
		handle_delete(response, *location, target.path);
		return;
	default:
		response.convert_to_error(501, location);
		return;
	}
}
