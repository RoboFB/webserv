/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:58:02 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/30 15:18:33 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include "logging.hpp"
#include "testing.hpp"


int main(int argc, const char *argv[])
{
	// first_website_copy_of_internet();
	first_website();
	try
	{
		parsing_start(argc, argv);
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

