/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParserStructs.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 18:36:32 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 13:07:50 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AddrInfoPtr.hpp"
#include "Methods.hpp"
#include "CGIConfig.hpp"

#include <filesystem>
#include <optional>
#include <vector>

struct DefaultConfigParser
{
		std::vector<int> error_codes; // todo
		std::vector<std::filesystem::path> indexs_paths;
		CGIConfigMap cgi_interpers;
		std::optional<std::filesystem::path> error_page_path; // todo
		std::optional<std::filesystem::path> root_path;
		std::optional<std::filesystem::path> return_path; // todo
		std::optional<size_t> client_max_body_size;
		std::optional<int> return_code; // todo
		std::optional<bool> autoindex;
		Methods limit_except;

		DefaultConfigParser() : limit_except(Methods::NONE) {};
};

struct LocationConfigParser : public DefaultConfigParser
{
		std::string location_path;

		LocationConfigParser() : DefaultConfigParser() {};
};

struct ServerConfigParser : public DefaultConfigParser
{
		std::vector<LocationConfigParser> locations_confs;

		AddrInfoPtr listen; // address:port linked list of listen addresses
		// std::string					server_name; // later need to be
		// implemented on header level (www.example.com, example.com, etc.)

		ServerConfigParser() : DefaultConfigParser(), listen(nullptr) {};
};

struct HttpConfigParser : public DefaultConfigParser
{
		std::vector<ServerConfigParser> servers_confs;

		HttpConfigParser() : DefaultConfigParser() {};
};

struct MainConfigParser
{
		HttpConfigParser http_conf;

		MainConfigParser(int argc, const char *argv[]);

	private:
		void inherit_all_config();
};
