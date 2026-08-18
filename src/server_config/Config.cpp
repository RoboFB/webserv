/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 12:29:08 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/18 19:43:11 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

#include "printing.hpp"

Config::Config(const std::filesystem::path &config_file_path) : config_file_path_(config_file_path),
	main_conf(),
	http_conf(main_conf.sub_conf),
	server_confs(http_conf.sub_confs)
{
	try
	{
		std::string source = file_to_string(config_file_path);
		std::vector<Token> tokens = string_to_tokens(source);
		if (tokens.empty())
			throw ConfigParseException("Empty configuration file.");
		
		debug_print_tokens(tokens);
		
	
		std::vector<Token>::const_iterator head_token = tokens.cbegin();
		fill_main(head_token);

	}
	catch(const ConfigParseException& e)
	{
		throw ConfigParseException(config_file_path_.string() + ":" + e.what());
	}
	return;
}

Config::~Config() {}


std::string Config::to_string() const
{
	std::stringstream config_stream;

	config_stream << "# " << config_file_path_.string() << "\n\n" << main_conf;
	return config_stream.str();
}




const ServerConfig & Config::get_server_context(size_t index) const
{
	return main_conf.sub_conf.sub_confs.at(index);
}
