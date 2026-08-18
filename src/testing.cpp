/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 13:39:09 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/18 15:32:08 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "testing.hpp"

#include "logging.hpp"

void test_logging(void)
{
	LOG(LOG_DEBUG, "Server is initializing...");
	LOG(LOG_INFO, "Starting the web server...");
	LOG(LOG_WARN, "This is a warning message.");
	LOG(LOG_ERROR, "An error occurred while starting the server.");
}
