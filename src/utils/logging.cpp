/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logging.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 12:34:10 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/18 18:57:20 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "logging.hpp"

# include "colors.hpp"
# include <iostream>

// Logging implementation: errors on 2, has coloring, and uses file, line, fuction on debugging
void log_impl(const LogLevel level, const std::string &message,
	const char* file, const int line, const char* func)
{
	if ((level & LOG_LVL) == 0)
	{
		return;
	}
	
	std::ostream& output = (level == LOG_ERROR)? std::cerr : std::cout;
	
	switch (level)
	{
		case LOG_ERROR: output << RED    << "[ERROR] " << RESET ; break;
		case LOG_WARN:  output << YELLOW << "[WARN]  " << RESET ; break;
		case LOG_INFO:  output << GREEN  << "[INFO]  " << RESET ; break;
		case LOG_DEBUG: output << CYAN   << "[DEBUG] " << RESET ; break;
	}
	if (LOG_LVL & LOG_DEBUG )
	{
		output << file << ":" << line << " (" << func << "):\n";
	}
	output << message << "\n";
}
