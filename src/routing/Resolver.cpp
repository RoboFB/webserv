/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resolver.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 22:51:52 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 23:03:25 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Resolver.hpp"
#include "Message.hpp"
#include <system_error>

bool is_path_within_root(
	const std::filesystem::path& candidate,
	const std::filesystem::path& root)
{
	std::filesystem::path::const_iterator root_part = root.begin();
	std::filesystem::path::const_iterator candidate_part = candidate.begin();

	while (root_part != root.end())
	{
		if (candidate_part == candidate.end() || *candidate_part != *root_part)
			return false;
		++root_part;
		++candidate_part;
	}
	return true;
}

static ResolvedPath forbidden_path()
{
	ResolvedPath result;

	result.status = ResolveStatus::FORBIDDEN;
	return result;
}

static std::optional<std::string> location_suffix(
	const Location& location,
	const std::string& request_path)
{
	if (request_path.empty() || request_path[0] != '/')
		return std::nullopt;

	if (location.location_path == "/")
		return request_path.substr(1);

	if (request_path.compare(
			0,
			location.location_path.size(),
			location.location_path) != 0)
		return std::nullopt;

	const std::size_t suffix_start = location.location_path.size();

	if (request_path.size() > suffix_start &&
		request_path[suffix_start] != '/')
		return std::nullopt;

	if (request_path.size() == suffix_start)
		return "";

	return request_path.substr(suffix_start + 1);
}

static std::optional<std::filesystem::path> safe_relative_path(
	const std::string& encoded_suffix)
{
	std::string decoded_suffix;

	if (!percent_decode(encoded_suffix, decoded_suffix))
		return std::nullopt;

	const std::filesystem::path relative_path(decoded_suffix);

	if (relative_path.is_absolute())
		return std::nullopt;

	for (const std::filesystem::path& part : relative_path)
	{
		if (part == "..")
			return std::nullopt;
	}

	return relative_path;
}

static std::optional<std::filesystem::path> canonical_path(
	const std::filesystem::path& path)
{
	std::error_code error;
	const std::filesystem::path canonical =
		std::filesystem::weakly_canonical(path, error);

	if (error)
		return std::nullopt;

	return canonical;
}

ResolvedPath resolve_resource_path(
	const Location& location,
	const std::string& request_path)
{
	const std::optional<std::string> suffix =
		location_suffix(location, request_path);

	if (!suffix)
		return forbidden_path();

	const std::optional<std::filesystem::path> relative_path =
		safe_relative_path(*suffix);

	if (!relative_path)
		return forbidden_path();

	const std::optional<std::filesystem::path> root =
		canonical_path(location.root_path);

	if (!root)
		return ResolvedPath();

	const std::optional<std::filesystem::path> candidate =
		canonical_path(*root / *relative_path);

	if (!candidate)
		return ResolvedPath();

	if (!is_path_within_root(*candidate, *root))
		return forbidden_path();

	ResolvedPath result;
	result.status = ResolveStatus::OK;
	result.path = *candidate;
	return result;
}
