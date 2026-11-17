/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_args.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 10:48:31 by rgohrig           #+#    #+#             */
/*   Updated: 2026/11/17 14:37:15 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "normalFucntions.hpp"

const std::filesystem::path check_input_args(int argc, const char *argv[])
{
	if (argc != 2)
	{
		throw std::runtime_error("wrong number of arguments.\n"
								 "./webserv [configuration file]\n");
	}
	return (argv[1]);
}
