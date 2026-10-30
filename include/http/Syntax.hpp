/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Syntax.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 00:09:05 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/29 13:37:54 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <optional>
#include <string>

struct HeaderField
{
		std::string name;
		std::string normalized_name;
		std::string value;
};

std::string ascii_lowercase(const std::string &input);

std::string trim_whitespace(const std::string &input);

bool parse_decimal_size(const std::string &text, std::size_t &value);

std::optional<HeaderField> parse_header_field(const std::string &line);
