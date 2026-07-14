/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 14:53:44 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/14 13:07:38 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>

# include <filesystem>
# include <unordered_map>
# include <vector>

/* 
special characters:
	- # comments
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

};

struct Token
{
    TokenType type;
    std::string value; // the word or the character itself

    size_t line;
    size_t column;
};

static const std::unordered_map<std::string, TokenType> g_keyword_tokens = {
    {"server", TokenType::WORD},
    {"location", TokenType::WORD},
    {"listen", TokenType::WORD},
    {"root", TokenType::WORD},
    {"index", TokenType::WORD},
    {"error_page", TokenType::WORD}
};




struct ErrorPageContext
{
	std::vector<int> error_codes;
	std::string error_page_path;
};

struct DefaultContext
{
	ErrorPageContext				error_page;
	size_t							client_max_body_size;
	bool							autoindex;
	std::filesystem::path			root;
	std::vector<std::filesystem::path>	index;

};


struct LocationContext : public DefaultContext
{
	int								return_code;

};

struct ServerContext : public DefaultContext
{
	std::vector<LocationContext>	location_context;
	std::vector<int>				listen; // address:port
	std::string						server_name;
	int								return_code;
	std::vector<std::string>		limit_except;

};

struct HttpContext : public DefaultContext
{
	std::vector<ServerContext>		server_context;
};

struct MainContext
{
	HttpContext http_context;
};




// main parsing functions
int parsing_start(int argc, const char *argv[]);
int main_parsing(const std::string &config_file_path_name);

// lexer functions
char get_next(std::string::const_iterator &head, size_t &line, size_t &column);
std::string file_to_string(const std::filesystem::path &config_file_path);
std::vector<Token> string_to_tokens(const std::string &source, const std::string &config_file_path_name);

std::ostream &operator<<(std::ostream &os, const TokenType &token);
std::ostream &operator<<(std::ostream &os, const Token &token);
void print_parsing_error(const Token &token, const std::string &filename);
