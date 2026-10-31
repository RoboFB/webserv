/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_parsing.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 16:19:19 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/30 16:32:30 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include <charconv>

// value & flag (uses bit and comparison)
constexpr bool ConfigParser::has_flag(ConfigContext value, ConfigContext flag)
{
	return (static_cast<uint>(value) & static_cast<uint>(flag));
}

// gets on token and move head, returns at the end eof if it is a  second time
// callet i thros a error
Token ConfigParser::get_next_token(
	std::vector<Token>::const_iterator &head_token)
{
	Token current_token = *head_token;

	if (current_token.type == TokenType::END_OF_FILE)
		throw ConfigParserException("unexpected end of file", current_token);
	++head_token;
	return current_token;
}

// gets on token and move head, returns at the end eof if it is a  second time
// callet i thros a error
void ConfigParser::is_at_end_of_file(
	std::vector<Token>::const_iterator &head_token)
{
	Token current_token = *head_token;

	if (current_token.type != TokenType::END_OF_FILE)
		throw ConfigParserException("no end of file reached", current_token);
}

// gets on token and move head
Token ConfigParser::get_next_word(
	std::vector<Token>::const_iterator &head_token)
{
	Token token = get_next_token(head_token);

	if (token.type != TokenType::WORD)
		throw ConfigParserException("expected a word", token);

	return token;
}

void ConfigParser::skip_next(std::vector<Token>::const_iterator &head_token,
							 TokenType expected_type)
{
	Token token = get_next_token(head_token);

	if (token.type != expected_type)
		throw ConfigParserException("expected a different token type", token);

	return;
}

ConfigParser::DirectiveLookup
ConfigParser::find_directive_lookup(const Token &token, ConfigContext context)
{
	if (token.type != TokenType::WORD)
		throw ConfigParserException("expected a word", token);
	for (auto lookup : directive_lookup)
	{
		if (has_flag(lookup.allowed_contexts, context) &&
			lookup.word == token.word)
			return lookup;
	}
	throw ConfigParserException("unknown word, not found in list", token);
}

bool ConfigParser::is_string_to_int(const std::string &string, int &result)
{
	const char *end = string.data() + string.size();
	auto [ptr, ec] = std::from_chars(string.data(), end, result);
	return ec == std::errc{} && ptr == end;
}

// Returns -1 if invalid, otherwise the number
int ConfigParser::validate_port(const std::string &str)
{
	int result = 0;
	if (!is_string_to_int(str, result))
		return -1;
	if (result < 0 || result > 65535)
		return -1;
	return result;
}
