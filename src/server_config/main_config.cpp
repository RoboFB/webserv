/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_config.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 10:48:31 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/13 16:47:03 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

#include "testing.hpp"

#include <iostream>
#include <filesystem>

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



int main_parsing(const std::string &config_file_path_name)
{
	std::filesystem::path config_file_path = config_file_path_name;
	
	std::string source = file_to_string(config_file_path);
	std::vector<Token> tokens = string_to_tokens(source, config_file_path.string());
	
	MainContext config = tokens_to_config(tokens);

	return (0);
}