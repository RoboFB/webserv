/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 12:29:08 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/19 16:51:17 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

#include "printing.hpp"

Config::Config(const std::filesystem::path &config_file_path) : 
	file_path_(config_file_path),
	main_conf_()
{
	try
	{
		complete_string_ = file_to_string(config_file_path);
		complete_tokens_ = string_to_tokens(complete_string_);
		if (complete_tokens_.empty())
			throw ConfigParseException("Empty configuration file.");
		
		debug_print_tokens(complete_tokens_);
		
	
		std::vector<Token>::const_iterator head_token = complete_tokens_.cbegin();
		fill_main(head_token);
		inherit_main_to_http(main_conf_, main_conf_.http_conf);

	}
	catch(const ConfigParseException& e)
	{
		throw ConfigParseException(file_path_.string() + ":" + e.what());
	}
	return;
}

Config::~Config() {}


std::string Config::to_string() const
{
	std::stringstream config_stream;

	config_stream << "# " << file_path_.string() << "\n\n" << main_conf_;
	return config_stream.str();
}




const ServerConfig & Config::get_server_context(size_t index) const
{
	return main_conf_.http_conf.servers_confs.at(index);
}
