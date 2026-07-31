/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:58:02 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/31 18:46:07 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "logging.hpp"
#include "testing.hpp"


int main(int argc, const char *argv[])
{
	// first_website_copy_of_internet();
	// first_website();
	try
	{
		const std::filesystem::path config_file = check_input_args(argc, argv);
		Config config(config_file);
		

		//TODO: make good webserver loop and system calls etc. 
		// first_website_copy_of_internet();
	}
	catch (const std::exception& e) 
	{
		LOG(LOG_ERROR, e.what());
		return EXIT_FAILURE;
	}
	catch (...) 
	{
		LOG(LOG_ERROR, "Unknown exception occurred that is not derived from std::exception.");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

