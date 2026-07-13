/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 14:53:44 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/09 19:14:31 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>

# include <filesystem>


/* 
specil characters:
	- # cmments
	- { and } block
	- ; end of line


path characters:
	- / path separator
	- . current directory
	- .. parent directory
	- ~ home directory?

whitespace characters:
	- space
	- tab
	- newline

*/


enum class TokenType
{
    WORD,          // server, listen, 8080, /var/www, GET, etc.

    OPEN_BRACE,    // {
    CLOSE_BRACE,   // }

    SEMICOLON,     // ;

    END_OF_FILE    // end marker
};

struct Token
{
    TokenType type;
    std::string value;

    size_t line;
    size_t column;
};



class Lexer
{
	public:
		Lexer();
		~Lexer();
		Lexer(const Lexer &other);
		Lexer &operator=(const Lexer &other);
		
	private:
	
};


std::ostream &operator<<(std::ostream &os, const Token &token);
void file_to_tokens(std::filesystem::path &config_file_path);
bool skip_whitespaces(std::string_view source, size_t &line, size_t &column);
void print_parsing_error(const Token &token, const std::string &filename);