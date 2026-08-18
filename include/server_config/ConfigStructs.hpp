/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStrucks.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 18:36:32 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/18 18:37:49 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AddrInfoPtr.hpp"
#include "Methods.hpp"
#include <filesystem>
#include <vector>

# define BODY_SIZE_FACTOR 1024 // ein bytes steep
# define BODY_SIZE_B 1 // byte
# define BODY_SIZE_K (BODY_SIZE_FACTOR * BODY_SIZE_B) // kilobyte
# define BODY_SIZE_M (BODY_SIZE_FACTOR * BODY_SIZE_K) // megabyte
# define BODY_SIZE_G (BODY_SIZE_FACTOR * BODY_SIZE_M) // gigabyte
# define BODY_SIZE_DEFAULT (1 * BODY_SIZE_M) // 1MB


struct DefaultConfig
{
	std::vector<int>				error_codes; //todo
	std::filesystem::path			error_page_path; //todo
    size_t							client_max_body_size;
    bool							autoindex;
    std::filesystem::path			root_path;
    std::vector<std::filesystem::path>	indexs_paths;
	int								return_code; //todo
	std::filesystem::path			return_path; //todo
	Methods					limit_except;
	
	DefaultConfig() : client_max_body_size(BODY_SIZE_DEFAULT), autoindex(false), return_code(0), limit_except(Methods::NONE) {};
};


struct LocationConfig : public DefaultConfig
{
	std::filesystem::path					location_path;

	LocationConfig() : DefaultConfig() {};
};



struct ServerConfig : public DefaultConfig
{
    std::vector<LocationConfig>	sub_confs;

	AddrInfoPtr					listen; // address:port linked list of listen addresses
    // std::string					server_name; // later need to be implemented on header level (www.example.com, example.com, etc.)




	ServerConfig() : DefaultConfig() , listen(nullptr) {};
};

struct HttpConfig : public DefaultConfig
{
    std::vector<ServerConfig>		sub_confs;

	HttpConfig() : DefaultConfig() {};
};

struct MainConfig
{
    HttpConfig sub_conf;
};