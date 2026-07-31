/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParseException.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 15:10:40 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/30 15:19:28 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdexcept>

struct Token;

class ConfigParseException : public std::runtime_error
{
    public:
        ConfigParseException(const std::string& error_message, const Token& error_token);
        ConfigParseException(const std::string& error_message)
            : std::runtime_error(error_message)
        {}
};