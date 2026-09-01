/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStructs.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 18:36:32 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/01 16:51:39 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AddrInfoPtr.hpp"
#include "Methods.hpp"

#include <filesystem>
#include <optional>
#include <vector>

struct DefaultConfig
{
		std::vector<int> error_codes; // todo
		std::vector<std::filesystem::path> indexs_paths;
		std::optional<std::filesystem::path> error_page_path; // todo
		std::optional<std::filesystem::path> root_path;
		std::optional<std::filesystem::path> return_path; // todo
		std::optional<size_t> client_max_body_size;
		std::optional<int> return_code; // todo
		std::optional<bool> autoindex;
		Methods limit_except;

		DefaultConfig() : limit_except(Methods::NONE) {};
};

struct LocationConfig : public DefaultConfig
{
		std::filesystem::path location_path;

		LocationConfig() : DefaultConfig() {};
};

struct ServerConfig : public DefaultConfig
{
		std::vector<LocationConfig> locations_confs;

		AddrInfoPtr listen; // address:port linked list of listen addresses
		// std::string					server_name; // later need to be
		// implemented on header level (www.example.com, example.com, etc.)

		ServerConfig() : DefaultConfig(), listen(nullptr) {};
};

struct HttpConfig : public DefaultConfig
{
		std::vector<ServerConfig> servers_confs;

		HttpConfig() : DefaultConfig() {};
};

struct MainConfig
{
		HttpConfig http_conf;
};

void inherit_all_config(MainConfig &parent_config);