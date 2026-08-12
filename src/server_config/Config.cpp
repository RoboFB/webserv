/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 12:29:08 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/07 20:28:33 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

#include "logging.hpp"
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
		LOG(LOG_DEBUG, "after  fill_main_context");
		

		debug_print_config(main_conf); // todo: remove debug print or clean it up to part of th e class
		
	}
	catch(const ConfigParseException& e)
	{
		throw ConfigParseException(config_file_path_.string() + ":" + e.what());
	}
	return;
}









const ServerConfig & Config::get_server_context(size_t index) const
{
	return main_conf.sub_conf.sub_confs.at(index);
}
