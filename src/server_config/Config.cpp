/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 12:29:08 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/25 13:56:22 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

#include "logging.hpp"
#include "printing.hpp"

Config::Config(const std::filesystem::path &config_file_path,
			   MainConfig &main_conf)
	: config_file_path_(config_file_path), main_conf_(main_conf)
{
	try
	{
		std::string complete_string_ = file_to_string(config_file_path);
		std::vector<Token> all_tokens_ = string_to_tokens(complete_string_);
		if (all_tokens_.empty())
			throw ConfigParseException("Empty configuration file.");
		std::vector<Token>::const_iterator head_token = all_tokens_.cbegin();
		fill_main(head_token);
		inherit_all_config(main_conf_);

		debug_print_tokens(all_tokens_);
	}
	catch (const ConfigParseException &e)
	{
		throw ConfigParseException(config_file_path_.string() + ":" + e.what());
	}
	LOG(LOG_DEBUG, this->to_string());
	return;
}

Config::~Config() {}

std::string Config::to_string() const
{
	std::stringstream config_stream;

	config_stream << "# " << config_file_path_.string() << "\n\n" << main_conf_;
	return config_stream.str();
}

const ServerConfig &Config::get_server_context(size_t index) const
{
	return main_conf_.http_conf.servers_confs.at(index);
}
