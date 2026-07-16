/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printig.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 13:59:22 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/16 12:37:15 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"


// print all tokens for debugging
void debug_print_tokens(const std::vector<Token> &tokens)
{
	if (!(LOG_LVL & LOG_DEBUG))
		return;
	std::stringstream log_all_tokens;

	for (const auto &token : tokens)
	{
		log_all_tokens << token << std::endl;
	}
	LOG(LOG_DEBUG, log_all_tokens.str());
}


std::ostream &operator<<(std::ostream &os, const TokenType &token)
{
	switch (token)
	{
	case TokenType::WORD:			os << "WORD       "; return os;
	case TokenType::OPEN_BRACE:		os << "OPEN_BRACE "; return os;
	case TokenType::CLOSE_BRACE:	os << "CLOSE_BRACE"; return os;
	case TokenType::SEMICOLON:		os << "SEMICOLON  "; return os;
	default: 						os << "UNKNOWN    "; return os;
	}
}

std::ostream &operator<<(std::ostream &os, const Token &token)
{
	os	<< token.type
		<< " "
    	<< token.line
        << ":"
        << token.column
		<< " \t"
		<< token.word;
		return os;
}


void print_parsing_error(const Token &token, const std::string &filename)
{
    std::stringstream error_message;

    error_message << filename
        << ":"
        << token.line
        << ":"
        << token.column
        << ": parsing error at: '"
        << token.word
		<< "'\n";

    LOG(LOG_ERROR, error_message.str());
}