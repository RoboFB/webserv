/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 14:53:44 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 13:07:50 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ConfigParserException.hpp"
// #include "AddrInfoPtr.hpp"

#include "ConfigParserStructs.hpp"

#include <string>
#include <string_view>

#include <filesystem>

#include <vector>
#include <array>

enum class TokenType
{
	WORD, // server, listen, 8080, /var/www, GET, etc.

	OPEN_BRACE, // {
	CLOSE_BRACE, // }

	SEMICOLON, // ;
	END_OF_FILE // end of file

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
	MAIN = 0b00001,
	HTTP = 0b00010,
	SERVER = 0b00100,
	LOCATION = 0b01000,
	DEFAULT = 0b01110
};

using TokenIterator = std::vector<Token>::const_iterator;

namespace ConfigParser
{
// lexer functions
char get_next(std::string::const_iterator &head, size_t &line, size_t &column);
std::string file_to_string(const std::filesystem::path &config_file_path);
std::vector<Token> string_to_tokens(const std::string &source);

// parser functions
Token get_next_token(std::vector<Token>::const_iterator &head_token);
Token get_next_word(std::vector<Token>::const_iterator &head_token);
void skip_next(std::vector<Token>::const_iterator &head_token,
			   TokenType expected_type);

constexpr bool has_flag(ConfigContext value, ConfigContext flag);

void is_at_end_of_file(std::vector<Token>::const_iterator &head_token);
int validate_port(const std::string &str);

void append_addrinfo(AddrInfoPtr &addr_list, const std::string &address_str,
					 const std::string &port_str, const Token &token);

size_t parse_max_body_size(std::string number_str, const Token &token);

struct DirectiveLookup
{
		ConfigContext allowed_contexts;
		std::string_view word;
		void (*fill_context)(TokenIterator &, DefaultConfigParser &);
};

DirectiveLookup find_directive_lookup(const Token &token,
									  ConfigContext context);

void fill_main(TokenIterator &head_token, MainConfigParser &fill_config);
void fill_http(TokenIterator &head_token, HttpConfigParser &fill_config);
void fill_server(TokenIterator &head_token,
				 std::vector<ServerConfigParser> &fill_config);
void fill_location(TokenIterator &head_token,
				   std::vector<LocationConfigParser> &fill_config);
void fill_listen(TokenIterator &head_token, DefaultConfigParser &fill_config);
// void fill_server_name(TokenIterator &head_token,
// 					  DefaultConfigPars &fill_config);
void fill_limit_except(TokenIterator &head_token,
					   DefaultConfigParser &fill_config);
void fill_error_page(TokenIterator &head_token,
					 DefaultConfigParser &fill_config);
void fill_client_max_body_size(TokenIterator &head_token,
							   DefaultConfigParser &fill_config);
void fill_autoindex(TokenIterator &head_token,
					DefaultConfigParser &fill_config);
void fill_root(TokenIterator &head_token, DefaultConfigParser &fill_config);
void fill_return(TokenIterator &head_token, DefaultConfigParser &fill_config);
void fill_index(TokenIterator &head_token, DefaultConfigParser &fill_config);
void fill_cgi(TokenIterator &head_token, DefaultConfigParser &fill_config);

// walks directive_lookup below for the directive names, so they're
// written once, not duplicated in printig.cpp
void print_default_directives(std::ostream &os,
							  const DefaultConfigParser &config, size_t indent);
static constexpr std::array<DirectiveLookup, 12> directive_lookup = {
	// sub_conf directives
	DirectiveLookup{ConfigContext::MAIN, "http", nullptr}, // fill_main_context,
	DirectiveLookup{ConfigContext::HTTP, "server",
					nullptr}, // fill_http_context,
	DirectiveLookup{ConfigContext::SERVER, "location",
					nullptr}, // fill_location_context,

	// Server only
	DirectiveLookup{ConfigContext::SERVER, "listen",
					&ConfigParser::fill_listen}, // fill_listen_context,
	// DirectiveLookup{ConfigContext::SERVER,    "server_name",
	// &Config::fill_server_name}, // fill_server_name_context,

	// Default
	DirectiveLookup{
		ConfigContext::DEFAULT, "limit_except",
		&ConfigParser::fill_limit_except}, // fill_limit_except_context,
	DirectiveLookup{ConfigContext::DEFAULT, "error_page",
					&ConfigParser::fill_error_page}, // fill_error_page_context,
	DirectiveLookup{
		ConfigContext::DEFAULT, "client_max_body_size",
		&ConfigParser::
			fill_client_max_body_size}, // fill_client_max_body_size_context,
	DirectiveLookup{ConfigContext::DEFAULT, "autoindex",
					&ConfigParser::fill_autoindex}, // fill_autoindex_context,
	DirectiveLookup{ConfigContext::DEFAULT, "root",
					&ConfigParser::fill_root}, // fill_root_context,
	DirectiveLookup{ConfigContext::DEFAULT, "index",
					&ConfigParser::fill_index}, // fill_index_context,
	DirectiveLookup{ConfigContext::DEFAULT, "return",
					&ConfigParser::fill_return},
	DirectiveLookup{ConfigContext::DEFAULT, "cgi", &ConfigParser::fill_cgi},
};

}; // namespace ConfigParser
