/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_parsing.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 16:19:19 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/06 15:07:08 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

// value & flag (uses bit and comparison)
constexpr bool Config::has_flag(ConfigContext value, ConfigContext flag)
{
    return (static_cast<uint>(value) & static_cast<uint>(flag));
}

// gets on token and move head, returns at the end eof if it is a  second time callet i thros a error
Token Config::get_next_token(std::vector<Token>::const_iterator &head_token)
{
	Token current_token = *head_token;

	if (current_token.type == TokenType::END_OF_FILE)
		throw ConfigParseException("unexpected end of file", current_token);
	++head_token;
    return current_token;
}

// gets on token and move head, returns at the end eof if it is a  second time callet i thros a error
void Config::is_at_end_of_file(std::vector<Token>::const_iterator &head_token)
{
	Token current_token = *head_token;

	if (current_token.type != TokenType::END_OF_FILE)
		throw ConfigParseException("no end of file reached", current_token);
}

// gets on token and move head
Token Config::get_next_word(std::vector<Token>::const_iterator &head_token)
{
	Token token = get_next_token(head_token);

	if (token.type != TokenType::WORD)
		throw ConfigParseException("expected a word", token);
	
    return token;
}

void Config::skip_next(std::vector<Token>::const_iterator &head_token, TokenType expected_type)
{
	Token token = get_next_token(head_token);

	if (token.type != expected_type)
		throw ConfigParseException("expected a different token type", token);

	return;
}

DirectiveLookup Config::find_directive_lookup(const Token & token, ConfigContext context)
{
	if (token.type != TokenType::WORD)
		throw ConfigParseException("expected a word", token);
	for (auto lookup : directive_lookup)
	{
		if (has_flag(lookup.allowed_contexts, context) && 
				lookup.word == token.word)
			return lookup;
	}
	throw ConfigParseException("unknown word, not found in list", token);
}




/* 
if number -> valid -> return number
if number -> out of range -> -1
if not number -> -2
if not used all of the string -> -3

*/
int validate_port(const std::string &str)
{
	try
	{
		size_t index = 0;
		int port = std::stoi(str, &index);
		if (index != str.size())
			return -3;
		if (port < 0 || port > 65535)
			return -1;
		return port;
	}
	catch (const std::exception &e)
	{
		return -2;
	}
}

// deprecated, not used anymore, but kept for reference
int validate_octet_address_part(const std::string &str)
{
	try
	{
		size_t index = 0;
		int port = std::stoi(str, &index);
		if (index != str.size())
			return -1;
		if (port < 0 || port > 255)
			return -1;
		return port;
	}
	catch (const std::exception &e)
	{
		return -1;
	}
}