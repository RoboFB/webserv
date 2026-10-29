/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 12:29:08 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/28 19:28:07 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

#include "printing.hpp"

ConfigPars::ConfigPars(MainConfigPars &main_conf) : main_conf_(main_conf) {}

ConfigPars::~ConfigPars() {}

void ConfigPars::pars_all(const std::filesystem::path &config_file_path)
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
		throw ConfigParseException(config_file_path.string() + ":" + e.what());
	}
}
