/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 14:53:44 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/31 12:08:30 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include "ConfigParseException.hpp"


# include <string>
# include <string_view>

# include <iostream> // IWYU pragma: export
# include <filesystem>

# include <vector>
# include <array>



enum class TokenType
{
    WORD,          // server, listen, 8080, /var/www, GET, etc.

    OPEN_BRACE,    // {
    CLOSE_BRACE,   // }

    SEMICOLON,     // ;
    END_OF_FILE    // end of file

};

struct Token
{
    TokenType type;
    std::string word; // the word or the character itself

    size_t line;
    size_t column;
};

enum class ConfigContext
{
    MAIN =     0b00001,
    HTTP =     0b00010,
    SERVER =   0b00100,
    LOCATION = 0b01000,
    DEFAULT =  0b01110
};


struct DefaultContext
{
	std::vector<int>				error_codes;
	std::filesystem::path			error_page_path;
    size_t							client_max_body_size;
    bool							autoindex;
    std::filesystem::path			root;
    std::vector<std::filesystem::path>	index;
	int								return_code;

	DefaultContext() : client_max_body_size(0), autoindex(false) {};
};


struct LocationContext : public DefaultContext
{
	LocationContext() : DefaultContext() {};
};

struct ServerContext : public DefaultContext
{
    std::vector<LocationContext>	location_context;
    std::vector<int>				listen; // address:port
    std::string						server_name;
    std::vector<std::string>		limit_except;

	ServerContext() : DefaultContext() {};
};

struct HttpContext : public DefaultContext
{
    std::vector<ServerContext>		server_context;

	HttpContext() : DefaultContext() {};
};

struct MainContext
{
    HttpContext http_context;
};




// using HandlerContext = void (*)(MainContext& config, std::vector<Token>::const_iterator& head);

// parser
// void fill_main_context(MainContext &config, std::vector<Token>::const_iterator head);
// void fill_http_context(HttpContext &config, std::vector<Token>::const_iterator head);
// void fill_location_context(LocationContext &config, std::vector<Token>::const_iterator head);
// void fill_limit_except_context(std::vector<std::string> &config, std::vector<Token>::const_iterator head);

// void fill_error_page_context(std::string &config, std::vector<Token>::const_iterator head);
// void fill_client_max_body_size_context(size_t &config, std::vector<Token>::const_iterator head);
// void fill_autoindex_context(HttpContext &config, std::vector<Token>::const_iterator head);
// void fill_root_context(HttpContext &config, std::vector<Token>::const_iterator head);
// void fill_index_context(HttpContext &config, std::vector<Token>::const_iterator head);
// void fill_listen_context(HttpContext &config, std::vector<Token>::const_iterator head);
// void fill_server_name_context(HttpContext &config, std::vector<Token>::const_iterator head);
// void fill_return_context(HttpContext &config, std::vector<Token>::const_iterator head);




struct DirectiveLookup
{
    ConfigContext allowed_contexts;
    std::string_view word;
	// std::function<void(MainContext&, std::vector<Token>::const_iterator&)> function;
    TokenType terminator;
};

static constexpr std::array<DirectiveLookup, 12> g_directive_lookup = {
        DirectiveLookup{ConfigContext::MAIN,      "http",                     TokenType::OPEN_BRACE}, // fill_main_context,
        DirectiveLookup{ConfigContext::HTTP,      "server",                   TokenType::OPEN_BRACE}, // fill_http_context,
        DirectiveLookup{ConfigContext::SERVER,    "location",                 TokenType::OPEN_BRACE}, // fill_location_context,
        DirectiveLookup{ConfigContext::LOCATION,  "limit_except",             TokenType::OPEN_BRACE}, // fill_limit_except_context,
        DirectiveLookup{ConfigContext::DEFAULT,   "error_page",               TokenType::SEMICOLON}, // fill_error_page_context,
        DirectiveLookup{ConfigContext::DEFAULT,   "client_max_body_size",     TokenType::SEMICOLON}, // fill_client_max_body_size_context,
        DirectiveLookup{ConfigContext::DEFAULT,   "autoindex",                TokenType::SEMICOLON}, // fill_autoindex_context,
        DirectiveLookup{ConfigContext::DEFAULT,   "root",                     TokenType::SEMICOLON}, // fill_root_context,
        DirectiveLookup{ConfigContext::DEFAULT,   "index",                    TokenType::SEMICOLON}, // fill_index_context,
        DirectiveLookup{ConfigContext::SERVER,    "listen",                   TokenType::SEMICOLON}, // fill_listen_context,
        DirectiveLookup{ConfigContext::SERVER,    "server_name",              TokenType::SEMICOLON}, // fill_server_name_context,
        DirectiveLookup{ConfigContext::SERVER,    "return",                   TokenType::SEMICOLON}, // fill_return_context,
};












// main parsing functions
int parsing_start(int argc, const char *argv[]);
int main_parsing(const std::filesystem::path &config_file_path);

// lexer functions
char get_next(std::string::const_iterator &head, size_t &line, size_t &column);
std::string file_to_string(const std::filesystem::path &config_file_path);
std::vector<Token> string_to_tokens(const std::string &source);








void fill_location_context(MainContext &fill_config,
					std::vector<Token>::const_iterator &head_token);

void fill_server_context(MainContext &fill_config,
					std::vector<Token>::const_iterator &head_token);

void fill_http_context(MainContext &fill_config,
					std::vector<Token>::const_iterator &head_token);

void fill_main_context(	MainContext &fill_config,
					std::vector<Token>::const_iterator &head_token);

