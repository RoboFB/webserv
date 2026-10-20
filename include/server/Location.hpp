/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 13:34:03 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/18 23:51:46 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ConfigStructs.hpp"
#include "Methods.hpp"
#include "CGIConfig.hpp"
#include <filesystem>
#include <vector>

class Location
{
	public:
		std::string location_path;
		std::vector<int> error_codes; // todo
		std::vector<std::filesystem::path> indexs_paths;
		std::filesystem::path error_page_path; // todo
		std::filesystem::path root_path;
		std::filesystem::path return_path; // todo
		std::optional<CGIConfig> cgi;
		size_t client_max_body_size;
		int return_code; // todo
		bool autoindex;
		Methods limit_except;

		Location(const LocationConfig &from);
};
