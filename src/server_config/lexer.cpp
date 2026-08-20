/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 17:42:37 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/20 19:25:06 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <fstream>
#include "ConfigParseException.hpp"
#include "MagicValues.hpp"

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
	
	try {
		std::uintmax_t size = std::filesystem::file_size(config_file_path);
		if (size > MAX_CONFIG_FILE_SIZE)
			throw std::runtime_error("Config file too large: " + config_file_path.string());
    }
    catch (std::filesystem::filesystem_error& e) {
		throw std::runtime_error("Could not get file size: " + config_file_path.string());
    }

	return std::string {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
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
			{
				auto &last_token = config_lines.back();
				last_token.word += c;
				if (last_token.word.length() > MAX_TOKEN_LENGTH)
					throw ConfigParseException("Token too long", last_token);
			}
			else
				config_lines.push_back({TokenType::WORD, std::string{c}, line, column});
			is_word = true;
		}
	}
	if (brace_level > 0)
		throw ConfigParseException("Missing closing brace", (Token){TokenType::CLOSE_BRACE, "}", line, column});

	config_lines.push_back({TokenType::END_OF_FILE, "END_OF_FILE", line, column});


	return config_lines;
}


