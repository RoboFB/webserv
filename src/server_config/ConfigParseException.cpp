/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParseException.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 15:10:40 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/06 15:09:02 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

ConfigParseException::ConfigParseException(const std::string& error_message, const Token& error_token)
    : std::runtime_error(
        std::to_string(error_token.line) + ":" +
        std::to_string(error_token.column) + ": " +
        error_message + " at token: '" +
        error_token.word + "'")
{}