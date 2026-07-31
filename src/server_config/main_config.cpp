/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_config.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 10:48:31 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/31 18:14:35 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"


const std::filesystem::path check_input_args(int argc, const char *argv[])
{
	if (argc != 2)
	{
		throw std::runtime_error (
			"wrong number of arguments.\n"
			"./webserv [configuration file]\n"
			"./webserv -t"
			"./webserv --test\n");
	}
	if (argv[1] == std::string_view("-t") || (argv[1] == std::string_view("--test")))
	{
		return "./config/hard_tests.conf";
	}
	return argv[1];
}


