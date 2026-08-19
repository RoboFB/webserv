/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStructs.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 18:36:32 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/19 18:04:21 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AddrInfoPtr.hpp"
#include "Methods.hpp"
#include <filesystem>
#include <optional>
#include <vector>

# define BODY_SIZE_FACTOR 1024 // ein bytes steep
# define BODY_SIZE_B 1 // byte
# define BODY_SIZE_K (BODY_SIZE_FACTOR * BODY_SIZE_B) // kibibyte
# define BODY_SIZE_M (BODY_SIZE_FACTOR * BODY_SIZE_K) // mebibyte
# define BODY_SIZE_G (BODY_SIZE_FACTOR * BODY_SIZE_M) // gibibyte
# define BODY_SIZE_DEFAULT (1 * BODY_SIZE_M) // 1MB

# define MAX_CONFIG_FILE_SIZE (1 * BODY_SIZE_G) // 1GB
# define MAX_TOKEN_LENGTH (1000) // 1000 characters


struct DefaultConfig
{
	std::vector<int>                                       error_codes; //todo
	std::optional<std::filesystem::path>                   error_page_path; //todo
    std::optional<size_t>                                  client_max_body_size;
    std::optional<bool>                                    autoindex;
    std::optional<std::filesystem::path>                   root_path;
    std::vector<std::filesystem::path>                     indexs_paths;
	std::optional<int>                                     return_code; //todo
	std::optional<std::filesystem::path>                   return_path; //todo
	Methods 				                               limit_except;
	
	DefaultConfig() : limit_except(Methods::NONE) {};
};


struct LocationConfig : public DefaultConfig
{
	std::filesystem::path					location_path;

	LocationConfig() : DefaultConfig() {};
};



struct ServerConfig : public DefaultConfig
{
    std::vector<LocationConfig>	locations_confs;

	AddrInfoPtr					listen; // address:port linked list of listen addresses
    // std::string					server_name; // later need to be implemented on header level (www.example.com, example.com, etc.)




	ServerConfig() : DefaultConfig() , listen(nullptr) {};
};

struct HttpConfig : public DefaultConfig
{
    std::vector<ServerConfig>		servers_confs;

	HttpConfig() : DefaultConfig() {};
};

struct MainConfig
{
    HttpConfig http_conf;
};