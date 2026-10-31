/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 13:34:03 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 19:47:07 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ErrorPage.hpp"
#include "Methods.hpp"
#include "CGIConfig.hpp"
#include <filesystem>
#include <optional>
#include <vector>

struct LocationConfigParser;

class Location
{
	public:
		// location
		std::string location_path;

		// from default waterfall
		std::vector<std::filesystem::path> indexs_paths;
		CGIConfigMap cgi_interpers;
		ErrorPages error_pages;
		std::optional<std::pair<int, std::string>> redirect_page;

		std::filesystem::path root_path;
		size_t client_max_body_size;
		bool autoindex;
		Methods limit_except;

		Location(const LocationConfigParser &from);
};
