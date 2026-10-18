/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Syntax.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 00:08:31 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/19 00:21:56 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Syntax.hpp"
#include <algorithm>
#include <cctype>
#include <limits>

static bool is_valid_field_name(const std::string& name)
{
	static const std::string allowed_special_characters =
		"!#$%&'*+-.^_`|~";

	if (name.empty())
		return false;

	for (std::string::const_iterator it = name.begin();
		it != name.end(); ++it)
	{
		const unsigned char character =
			static_cast<unsigned char>(*it);

		if (!std::isalnum(character)
			&& allowed_special_characters.find(*it)
				== std::string::npos)
		{
			return false;
		}
	}
	return true;
}


std::string ascii_lowercase(const std::string& input)
{
	std::string result = input;

	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char character)
		{
			return static_cast<char>(std::tolower(character));
		});
	return result;
}

std::string trim_whitespace(const std::string& input)
{
	const std::size_t first = input.find_first_not_of(" \t");

	if (first == std::string::npos)
		return "";

	const std::size_t last = input.find_last_not_of(" \t");

	return input.substr(first, last - first + 1);
}

bool parse_decimal_size(const std::string& text, std::size_t& value)
{
	if (text.empty())
		return false;

	value = 0;

	for (std::string::const_iterator it = text.begin();
		it != text.end(); ++it)
	{
		const unsigned char character =
			static_cast<unsigned char>(*it);

		if (!std::isdigit(character))
			return false;

		const std::size_t digit = character - '0';

		if (value > (std::numeric_limits<std::size_t>::max() - digit) / 10)
			return false;

		value = value * 10 + digit;
	}
	return true;
}

std::optional<HeaderField>
parse_header_field(const std::string& line)
{
	const std::size_t colon = line.find(':');

	if (colon == std::string::npos || colon == 0)
		return std::nullopt;

	HeaderField field;

	field.name = line.substr(0, colon);

	if (!is_valid_field_name(field.name))
		return std::nullopt;

	field.normalized_name = ascii_lowercase(field.name);
	field.value = trim_whitespace(line.substr(colon + 1));

	return field;
}