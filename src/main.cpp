/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 14:58:02 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/13 13:40:33 by rgohrig          ###   ########.fr       */
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
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	catch (...) 
	{
		std::cerr << "Unknown exception occurred." << std::endl;
		return 1;
	}
	return EXIT_SUCCESS;
}

