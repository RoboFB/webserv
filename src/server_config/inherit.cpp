/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inherit.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 16:51:33 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/28 19:19:04 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "ConfigStructs.hpp"
#include "normalFucntions.hpp"

template <typename T>
static void h_inherit_field(const std::optional<T> &parent,
							std::optional<T> &child)
{
	if (parent && !child)
		child = parent;
}

template <typename T>
static void h_inherit_field(const std::vector<T> &parent, std::vector<T> &child)
{
	if (child.empty() && !parent.empty())
		child = parent;
}

static void h_inherit_field(const CGIConfigMap &parent, CGIConfigMap &child)
{
	// inherit single values
	//  for (const auto &pair : parent)
	//  {
	//  	if (child.find(pair.first) == child.end())
	//  		child[pair.first] = pair.second;
	//  }

	// inherit only if child empty
	if (child.empty() && !parent.empty())
		child = parent;
}

static void h_inherit_field(const Methods &parent, Methods &child)
{
	if (child == Methods::NONE && parent != Methods::NONE)
		child = parent;
}

static void h_inherit_default_to_default(const DefaultConfigPars &parent,
										 DefaultConfigPars &child)
{
	h_inherit_field(parent.error_codes, child.error_codes);
	h_inherit_field(parent.error_page_path, child.error_page_path);
	h_inherit_field(parent.client_max_body_size, child.client_max_body_size);
	h_inherit_field(parent.autoindex, child.autoindex);
	h_inherit_field(parent.root_path, child.root_path);
	h_inherit_field(parent.indexs_paths, child.indexs_paths);
	h_inherit_field(parent.return_code, child.return_code);
	h_inherit_field(parent.return_path, child.return_path);
	h_inherit_field(parent.limit_except, child.limit_except);
	h_inherit_field(parent.cgi_interpers, child.cgi_interpers);
}

// set also the defaults at the end
static void h_inherit_server_to_location(const ServerConfigPars &parent,
										 LocationConfigPars &child)
{
	h_inherit_default_to_default(parent, child);

	// set_default_values(child);
}

static void h_inherit_http_to_server(const HttpConfigPars &parent,
									 ServerConfigPars &child)
{
	h_inherit_default_to_default(parent, child);

	for (auto &location : child.locations_confs)
	{
		h_inherit_server_to_location(child, location);
	}
}

static void h_inherit_main_to_http(const MainConfigPars &parent,
								   HttpConfigPars &child)
{
	(void)parent; // unused for now, but might be used in the future

	for (ServerConfigPars &server : child.servers_confs)
	{
		h_inherit_http_to_server(child, server);
	}
}

void inherit_all_config(MainConfigPars &parent_config)
{
	h_inherit_main_to_http(parent_config, parent_config.http_conf);
}

MainConfigPars::MainConfigPars(int argc, const char *argv[]) : http_conf()
{
	const std::filesystem::path config_file = check_input_args(argc, argv);
	ConfigPars config{*this};
	config.pars_all(config_file);
}
