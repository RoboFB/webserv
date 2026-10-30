/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParseException.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 15:10:40 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 12:58:15 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdexcept>

struct Token;

class ConfigParserException : public std::runtime_error
{
	public:
		ConfigParserException(const std::string &error_message,
							  const Token &error_token);
		ConfigParserException(const std::string &error_message)
			: std::runtime_error(error_message)
		{
		}
};