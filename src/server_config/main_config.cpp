/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_config.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 10:48:31 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/16 13:45:11 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

int parsing_start(int argc, const char *argv[])
{
	if (argc == 2 && argv[1] == std::string("-t"))
	{
		test_lexer();
		return 0;
	}
	if (argc != 2)
	{
		std::cerr << "wrong number of arguments.\n"
			"try as argument (char, int, float or double)\n";
		return 1;
	}

	return (main_parsing(argv[1]));
}




int main_parsing(const std::filesystem::path &config_file_path)
{
	try
	{
		std::string source = file_to_string(config_file_path);
		std::vector<Token> tokens = string_to_tokens(source);
	
		debug_print_tokens(tokens);

		if (tokens.empty())
			throw ConfigParseException("Empty configuration file.");


		MainContext config;
		fill_main_context(config, tokens.cbegin());
	
	
		
	}
	catch(const ConfigParseException& e)
	{
		throw ConfigParseException(config_file_path.string() + ":" + e.what());
	}
	return (0);
}