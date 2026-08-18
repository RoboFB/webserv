/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logging.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 12:34:27 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/27 15:05:22 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

enum LogLevel {
	LOG_ERROR 	= 0b0001,
	LOG_WARN 	= 0b0010,
	LOG_INFO 	= 0b0100,
	LOG_DEBUG 	= 0b1000
};

# ifndef LOG_LVL
#  define LOG_LVL (LOG_INFO | LOG_WARN | LOG_ERROR)
# endif

#define LOG(level, msg) \
    log_impl(level, msg, __FILE__, __LINE__, __func__)

void log_impl(const LogLevel level, const std::string &message, const char *file, const int line, const char *func);
