/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 16:19:19 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/16 15:58:39 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

// gets on char out of iterator but dos not check for end of string;
// Token get_next_token(std::string::const_iterator &head)
// {

// 	if (head == source.end())
// 		return Token{TokenType::END_OF_FILE, "", line, column};
	
//     return c;
// }


constexpr bool has_flag(ConfigContext value, ConfigContext flag)
{
    return (static_cast<uint>(value) & static_cast<uint>(flag));
}

void fill_http_context(HttpContext &config, std::vector<Token>::const_iterator head)
{
	(void)config;
	while (head->type != TokenType::CLOSE_BRACE)
	{
		if (head->type != TokenType::WORD)
			throw ConfigParseException("element needs to be a word", *head);
		
		auto lookup = g_directive_lookup.cbegin();
		for (; lookup != g_directive_lookup.cend(); lookup++)
		{
			if (!has_flag(lookup->allowed_contexts, ConfigContext::HTTP))
				continue;
			if (lookup->word != head->word)
				continue;
			head++;
			if (lookup->terminator == TokenType::OPEN_BRACE)
			{
				
			}
			
			break;
		}
		if (lookup == g_directive_lookup.cend())
			throw ConfigParseException("unknown word, not found in list", *head);
			
		head++;
	}
	return;
}


void fill_main_context(MainContext &config, std::vector<Token>::const_iterator head)
{

	if (head->type != TokenType::WORD)
		throw ConfigParseException("first element needs to be 'http'", *head);
	if (head->word != "http")
		throw ConfigParseException("first element needs to be 'http'", *head);
	head++;

	if (head->type != TokenType::OPEN_BRACE)
		throw ConfigParseException("expected '{' after 'http'", *head);
	head++;

	fill_http_context(config.http_context, head);

	if (head->type != TokenType::CLOSE_BRACE)
		throw ConfigParseException("expected '}' after 'http' block", *head);
	head++;


	return;
}