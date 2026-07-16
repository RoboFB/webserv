/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:58:02 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/16 15:08:21 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

#include <iostream>



int main(int argc, const char *argv[])
{
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

