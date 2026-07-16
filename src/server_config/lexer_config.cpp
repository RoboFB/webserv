/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_config.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 17:42:37 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/16 12:37:15 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

// gets on char out of iterator but dos not check for end of string;
char get_next(std::string::const_iterator &head, size_t &line, size_t &column)
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
std::string file_to_string(const std::filesystem::path &config_file_path)
{
	std::ifstream file(config_file_path);
	if (!file)
		throw std::runtime_error("Could not open file: " + config_file_path.string());
	std::string source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

	return source;
}


// R: config object?
std::vector<Token> string_to_tokens(const std::string &source)
{
	std::vector<Token> config_lines;

	size_t line = 1, column = 0;
	std::string::const_iterator head = source.begin();

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
			while ( head != source.end() && get_next(head, line, column) != '\n')
				;
			continue;
		}
		else if (c == '{')
		{
			is_word = false;
			++brace_level;
			config_lines.push_back({TokenType::OPEN_BRACE, "{", line, column});
		}
		else if (c == '}')
		{
			is_word = false;
			--brace_level;
			config_lines.push_back({TokenType::CLOSE_BRACE, "}", line, column});
			if (brace_level < 0)
				throw ConfigParseException("Unexpected closing brace", config_lines.back());
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
		}
		else if (std::isspace(static_cast<unsigned char>(c)) && !is_quoted)
		{
			is_word = false;
			continue;
		}
		else
		{
			if (is_word)
				config_lines.back().word += c;
			else
				config_lines.push_back({TokenType::WORD, std::string{c}, line, column});
			is_word = true;
		}
	}
	if (brace_level > 0)
		throw ConfigParseException("Missing closing brace", (Token){TokenType::CLOSE_BRACE, "}", line, column});




	return config_lines;
}


