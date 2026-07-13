/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_config.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 17:42:37 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/09 19:38:50 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_config.hpp"

#include "logging.hpp"


#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>

// gets on char out of iterator but dos not check for end of string;
char get_next(std::string::iterator &head, size_t &line, size_t &column)
{
	char c = *head;
	if (c == '\0')
		return '\0';
	head++;
    if (c == '\n')
    {
        line++;
        column = 0;
    }
    else
    {
        column++;
    }
    return c;
}


// R: config object?
void file_to_tokens(std::filesystem::path &config_file_path)
{
	std::vector<Token> config_lines;

	std::ifstream file(config_file_path);
	if (!file)
		throw std::runtime_error("Could not open file");
	std::string source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};


	size_t line = 1, column = 0;
	std::string::iterator head = source.begin();

	bool is_quoted = false;
	bool is_word = false;
	int brace_level = 0;

	while (true)
	{
		char c = get_next(head, line, column);
		if (c == '\0')
			break;
		else if (c == '#')
		{
			is_word = false;
			while (get_next(head, line, column) != '\n' && head != source.end())
				;
			continue;
		}
		else if (c == '{')
		{
			is_word = false;
			brace_level++;
			config_lines.push_back({TokenType::OPEN_BRACE, "{", line, column});
		}
		else if (c == '}')
		{
			is_word = false;
			brace_level--;
			config_lines.push_back({TokenType::CLOSE_BRACE, "}", line, column});
			if (brace_level < 0)
				print_parsing_error(config_lines.back(), config_file_path.string());
		}
		else if (c == ';')
		{
			is_word = false;
			config_lines.push_back({TokenType::SEMICOLON, ";", line, column});
		}
		else if (c == '"')
		{
			is_word = false;
			is_quoted = !is_quoted;
			config_lines.push_back({TokenType::SEMICOLON, ";", line, column});
		}
		else if ((c == ' ' || c == '\t' || c == '\n') && !is_quoted)
		{
			is_word = false;
			continue;
		}
		else
		{
			if (is_word)
			{
				config_lines.back().value += c;
			}
			else
			{
				config_lines.push_back({TokenType::WORD, std::string{c}, line, column});
				is_word = true;
			}
		}
	}
	if (brace_level != 0)
		print_parsing_error((Token){TokenType::CLOSE_BRACE, "}", line, column}, config_file_path.string());


	std::stringstream log_all_tokens;
	for (const auto &token : config_lines)
	{
		log_all_tokens << token << std::endl;
	}
	LOG(LOG_DEBUG, log_all_tokens.str());



	return;
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
		<< token.value;
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
        << token.value
		<< "'\n";

    LOG(LOG_ERROR, error_message.str());
}
