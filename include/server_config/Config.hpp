/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 14:53:44 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/20 19:21:17 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include "ConfigParseException.hpp"
// #include "AddrInfoPtr.hpp"

#include "ConfigStructs.hpp"

# include <string>
# include <string_view>

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



class Config;
using TokenIterator = std::vector<Token>::const_iterator;

struct DirectiveLookup
{
    ConfigContext		allowed_contexts;
    std::string_view	word;
	void				(Config::*fill_context)(TokenIterator &, DefaultConfig &);
};


class Config
{
	public:
		Config(const Config &) = delete;
		Config(Config &&) = default;
		Config &operator=(const Config &) = delete;
		Config &operator=(Config &&) = delete;

		Config(const std::filesystem::path &config_file_path, MainConfig &main_conf);
		~Config();

		const ServerConfig &get_server_context(size_t index) const;

		std::string to_string() const;

		// walks directive_lookup below for the directive names, so they're written once, not duplicated in printig.cpp
		static void print_default_directives(std::ostream &os, const DefaultConfig &config, size_t indent);
		

	private:
		const std::filesystem::path	&config_file_path_;

		// config structs
		MainConfig &main_conf_;


		DirectiveLookup find_directive_lookup(const Token & token, ConfigContext context);

		constexpr bool has_flag(ConfigContext value, ConfigContext flag);

		Token get_next_token(std::vector<Token>::const_iterator &head_token);
		Token get_next_word(std::vector<Token>::const_iterator &head_token);
		void skip_next(std::vector<Token>::const_iterator &head_token, TokenType expected_type);
		void is_at_end_of_file(std::vector<Token>::const_iterator &head_token);
		int validate_port(const std::string &str);


		void append_addrinfo(AddrInfoPtr &addr_list, const std::string &address_str, const std::string &port_str, const Token &token);
		
		size_t parse_max_body_size(std::string number_str, const Token &token);
		
		void inherit_main_to_http(const MainConfig &parent_config, HttpConfig &child_config);
		void inherit_http_to_server(const HttpConfig &parent_config, ServerConfig &child_config);
		void inherit_server_to_location(const ServerConfig &parent_config, LocationConfig &child_config);
		void inherit_default_to_default(const DefaultConfig &parent_config, DefaultConfig &child_config);

		void set_default_values(DefaultConfig &child);


		void fill_main( TokenIterator &head_token);
		void fill_http( TokenIterator &head_token);
		void fill_server( TokenIterator &head_token);
		void fill_location(TokenIterator &head_token, std::vector<LocationConfig> &fill_config);
		void fill_listen(
			TokenIterator &head_token,
			DefaultConfig &fill_config);
		void fill_server_name(
			TokenIterator &head_token,
			DefaultConfig &fill_config);
		void fill_limit_except(
			TokenIterator &head_token,
			DefaultConfig &fill_config);
		void fill_error_page(
			TokenIterator &head_token,
			DefaultConfig &fill_config);
		void fill_client_max_body_size(
			TokenIterator &head_token,
			DefaultConfig &fill_config);
		void fill_autoindex(
			TokenIterator &head_token,
			DefaultConfig &fill_config);
		void fill_root(
			TokenIterator &head_token,
			DefaultConfig &fill_config);
		void fill_return(
			TokenIterator &head_token,
			DefaultConfig &fill_config);
		void fill_index(
			TokenIterator &head_token,
			DefaultConfig &fill_config);
				
		static constexpr std::array<DirectiveLookup, 12> directive_lookup = {
				// sub_conf directives
				DirectiveLookup{ConfigContext::MAIN,      "http",                     nullptr}, // fill_main_context,
				DirectiveLookup{ConfigContext::HTTP,      "server",                   nullptr}, // fill_http_context,
				DirectiveLookup{ConfigContext::SERVER,    "location",                 nullptr}, // fill_location_context,
		
				// Server only
				DirectiveLookup{ConfigContext::SERVER,    "listen",                   &Config::fill_listen}, // fill_listen_context,
				// DirectiveLookup{ConfigContext::SERVER,    "server_name",              &Config::fill_server_name}, // fill_server_name_context,
		
				// Default
				DirectiveLookup{ConfigContext::DEFAULT,  "limit_except",              &Config::fill_limit_except}, // fill_limit_except_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "error_page",                &Config::fill_error_page}, // fill_error_page_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "client_max_body_size",      &Config::fill_client_max_body_size}, // fill_client_max_body_size_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "autoindex",                 &Config::fill_autoindex}, // fill_autoindex_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "root",                      &Config::fill_root}, // fill_root_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "index",                     &Config::fill_index}, // fill_index_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "return",                    &Config::fill_return},
		};
};






// lexer functions
char get_next(std::string::const_iterator &head, size_t &line, size_t &column);
std::string file_to_string(const std::filesystem::path &config_file_path);
std::vector<Token> string_to_tokens(const std::string &source);

