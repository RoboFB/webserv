/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inherit.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/19 16:51:33 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 13:10:04 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "ConfigParserStructs.hpp"
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

static void h_inherit_default_to_default(const DefaultConfigParser &parent,
										 DefaultConfigParser &child)
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
static void h_inherit_server_to_location(const ServerConfigParser &parent,
										 LocationConfigParser &child)
{
	h_inherit_default_to_default(parent, child);
}

static void h_inherit_http_to_server(const HttpConfigParser &parent,
									 ServerConfigParser &child)
{
	h_inherit_default_to_default(parent, child);

	for (auto &location : child.locations_confs)
	{
		h_inherit_server_to_location(child, location);
	}
}

static void h_inherit_main_to_http(const MainConfigParser &parent,
								   HttpConfigParser &child)
{
	(void)parent; // unused for now, but might be used in the future

	for (ServerConfigParser &server : child.servers_confs)
	{
		h_inherit_http_to_server(child, server);
	}
}

void MainConfigParser::inherit_all_config()
{
	h_inherit_main_to_http(*this, http_conf);
}

MainConfigParser::MainConfigParser(int argc, const char *argv[]) : http_conf()
{
	const std::filesystem::path config_file = check_input_args(argc, argv);
	try
	{
		std::string complete_string_ =
			ConfigParser::file_to_string(config_file);
		std::vector<Token> all_tokens_ =
			ConfigParser::string_to_tokens(complete_string_);
		if (all_tokens_.empty())
			throw ConfigParserException("Empty configuration file.");
		std::vector<Token>::const_iterator head_token = all_tokens_.cbegin();
		ConfigParser::fill_main(head_token, *this);
		inherit_all_config();
	}
	catch (const ConfigParserException &e)
	{
		throw ConfigParserException(config_file.string() + ":" + e.what());
	}
}
