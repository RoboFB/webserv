/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resolver.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 22:52:07 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/20 18:26:31 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Location.hpp"
#include <filesystem>
#include <string>

enum class ResolveStatus
{
	OK,
	FORBIDDEN,
	INTERNAL_ERROR
};

struct ResolvedPath
{
		ResolveStatus status = ResolveStatus::INTERNAL_ERROR;
		std::filesystem::path path;
};

bool is_path_within_root(const std::filesystem::path &candidate,
						 const std::filesystem::path &root);

ResolvedPath resolve_resource_path(const Location &location,
								   const std::string &request_path);
