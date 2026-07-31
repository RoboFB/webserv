/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 14:53:44 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/31 18:12:54 by rgohrig          ###   ########.fr       */
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

using TokenIterator = std::vector<Token>::const_iterator;

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


struct DefaultConfig
{
	std::vector<int>				error_codes;
	std::filesystem::path			error_page_path;
    size_t							client_max_body_size;
    bool							autoindex;
    std::filesystem::path			root;
    std::vector<std::filesystem::path>	index;
	int								return_code;
	std::vector<std::string>		limit_except;
	
	DefaultConfig() : client_max_body_size(0), autoindex(false) {};
};


struct LocationConfig : public DefaultConfig
{


	LocationConfig() : DefaultConfig() {};
};

struct ServerConfig : public DefaultConfig
{
    std::vector<LocationConfig>	sub_confs;

    std::vector<int>				listen; // address:port
    std::string						server_name;

	ServerConfig() : DefaultConfig() {};
};

struct HttpConfig : public DefaultConfig
{
    std::vector<ServerConfig>		sub_confs;

	HttpConfig() : DefaultConfig() {};
};

struct MainConfig
{
    HttpConfig sub_conf;
};

class Config;



struct DirectiveLookup
{
    ConfigContext		allowed_contexts;
    std::string_view	word;
	void				(Config::*fill_context)(TokenIterator &, DefaultConfig &, const DirectiveLookup &);
};






class Config
{
	public:
		Config(const Config &) = delete;
		Config(Config &&) = default;
		Config &operator=(const Config &) = delete;
		Config &operator=(Config &&) = delete;

		Config(const std::filesystem::path &config_file_path);

		const ServerConfig &get_server_context(size_t index) const;

	private:
		std::filesystem::path config_file_path_;
		MainConfig main_conf;
		
		HttpConfig &http_conf;
		std::vector<ServerConfig> &server_confs;
		// std::vector<LocationContext> &default_contexts_;
		

		DirectiveLookup find_directive_lookup(const Token & token, ConfigContext context);

		constexpr bool has_flag(ConfigContext value, ConfigContext flag);

		Token get_next_token(std::vector<Token>::const_iterator &head_token);
		Token get_next_word(std::vector<Token>::const_iterator &head_token);
		void skip_next(std::vector<Token>::const_iterator &head_token, TokenType expected_type);

		
		void fill_main_context( TokenIterator &head_token);
		void fill_http_context( TokenIterator &head_token);
		void fill_server_context( TokenIterator &head_token);
		void fill_location_context(TokenIterator &head_token, std::vector<LocationConfig> &fill_config);
		void fill_listen_context(
			TokenIterator &head_token,
			DefaultConfig &fill_config,
			const DirectiveLookup &current_lookup);
		void fill_server_name_context(
			TokenIterator &head_token,
			DefaultConfig &fill_config,
			const DirectiveLookup &current_lookup);
		void fill_limit_except_context(
			TokenIterator &head_token,
			DefaultConfig &fill_config,
			const DirectiveLookup &current_lookup);
		void fill_error_page_context(
			TokenIterator &head_token,
			DefaultConfig &fill_config,
			const DirectiveLookup &current_lookup);
		void fill_client_max_body_size_context(
			TokenIterator &head_token,
			DefaultConfig &fill_config,
			const DirectiveLookup &current_lookup);
		void fill_autoindex_context(
			TokenIterator &head_token,
			DefaultConfig &fill_config,
			const DirectiveLookup &current_lookup);
		void fill_root_context(
			TokenIterator &head_token,
			DefaultConfig &fill_config,
			const DirectiveLookup &current_lookup);
		void fill_return_context(
			TokenIterator &head_token,
			DefaultConfig &fill_config,
			const DirectiveLookup &current_lookup);
		void fill_index_context(
			TokenIterator &head_token,
			DefaultConfig &fill_config,
			const DirectiveLookup &current_lookup);
				
		static constexpr std::array<DirectiveLookup, 12> directive_lookup = {
				// sub_conf directives
				DirectiveLookup{ConfigContext::MAIN,      "http",                     nullptr}, // fill_main_context,
				DirectiveLookup{ConfigContext::HTTP,      "server",                   nullptr}, // fill_http_context,
				DirectiveLookup{ConfigContext::SERVER,    "location",                 nullptr}, // fill_location_context,
		
				// Server only
				DirectiveLookup{ConfigContext::SERVER,    "listen",                   &Config::fill_listen_context}, // fill_listen_context,
				DirectiveLookup{ConfigContext::SERVER,    "server_name",              &Config::fill_server_name_context}, // fill_server_name_context,
		
				// Default
				DirectiveLookup{ConfigContext::DEFAULT,  "limit_except",              &Config::fill_limit_except_context}, // fill_limit_except_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "error_page",                &Config::fill_error_page_context}, // fill_error_page_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "client_max_body_size",      &Config::fill_client_max_body_size_context}, // fill_client_max_body_size_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "autoindex",                 &Config::fill_autoindex_context}, // fill_autoindex_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "root",                      &Config::fill_root_context}, // fill_root_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "index",                     &Config::fill_index_context}, // fill_index_context,
				DirectiveLookup{ConfigContext::DEFAULT,  "return",                    &Config::fill_return_context},
		};
};






// main parsing functions
const std::filesystem::path check_input_args(int argc, const char *argv[]);










// lexer functions
char get_next(std::string::const_iterator &head, size_t &line, size_t &column);
std::string file_to_string(const std::filesystem::path &config_file_path);
std::vector<Token> string_to_tokens(const std::string &source);

