/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   decode.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 20:01:03 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/20 19:22:25 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

int hex_value(char character)
{
	if (character >= '0' && character <= '9')
		return character - '0';
	if (character >= 'a' && character <= 'f')
		return character - 'a' + 10;
	if (character >= 'A' && character <= 'F')
		return character - 'A' + 10;
	return -1;
}

bool percent_decode(const std::string &input, std::string &output)
{
	output.clear();
	for (std::size_t index = 0; index < input.size(); ++index)
	{
		if (input[index] != '%')
		{
			output += input[index];
			continue;
		}
		if (index + 2 >= input.size())
			return (false);
		const int high = hex_value(input[index + 1]);
		const int low = hex_value(input[index + 2]);
		if (high < 0 || low < 0)
			return (false);
		const char decoded = static_cast<char>(high * 16 + low);
		if (decoded == '\0')
			return (false);
		output += decoded;
		index += 2;
	}
	return (true);
}
