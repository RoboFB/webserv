/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inherit.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 16:51:33 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/19 18:28:31 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

void Config::inherit_main_to_http(const MainConfig &parent, HttpConfig &child)
{
	(void)parent; // unused for now, but might be used in the future

	for (ServerConfig &server : child.servers_confs)
	{
    	inherit_http_to_server(child, server);
	}
}


void Config::inherit_http_to_server(const HttpConfig &parent, ServerConfig &child)
{
	inherit_default_to_default(parent, child);

	for (auto &location : child.locations_confs)
	{
		inherit_server_to_location(child, location);
	}
}

// set also the defaults at the end
void Config::inherit_server_to_location(const ServerConfig &parent, LocationConfig &child)
{
	inherit_default_to_default(parent, child);

	set_default_values(child);
}

template <typename T>
static void inherit_field(const std::optional<T> &parent, std::optional<T> &child)
{
	if (parent && !child)
		child = parent;
}

template <typename T>
static void inherit_field(const std::vector<T> &parent, std::vector<T> &child)
{
	if (child.empty() && !parent.empty())
		child = parent;
}

static void inherit_field(const Methods &parent, Methods &child)
{
	if (child == Methods::NONE && parent != Methods::NONE)
		child = parent;
}

void Config::inherit_default_to_default(const DefaultConfig &parent, DefaultConfig &child)
{
	inherit_field(parent.error_codes, child.error_codes);
	inherit_field(parent.error_page_path, child.error_page_path);
	inherit_field(parent.client_max_body_size, child.client_max_body_size);
	inherit_field(parent.autoindex, child.autoindex);
	inherit_field(parent.root_path, child.root_path);
	inherit_field(parent.indexs_paths, child.indexs_paths);
	inherit_field(parent.return_code, child.return_code);
	inherit_field(parent.return_path, child.return_path);
	inherit_field(parent.limit_except, child.limit_except);
}

void Config::set_default_values(DefaultConfig &child)
{
	if (child.error_codes.empty())
		child.error_codes.push_back(999); //todo
	if (!child.error_page_path)
		child.error_page_path = "./example"; //todo
	if (!child.client_max_body_size)
		child.client_max_body_size = BODY_SIZE_DEFAULT;
	if (!child.autoindex)
		child.autoindex = false;
	if (!child.root_path)
		child.root_path = "."; //todo
	if (child.indexs_paths.empty())
		child.indexs_paths.push_back("index.html"); //todo
	if (!child.return_code)
		child.return_code = 888; //todo
	if (!child.return_path)
		child.return_path = "./example"; //todo
	if (child.limit_except == Methods::NONE)
		child.limit_except = Methods::GET; //todo


}