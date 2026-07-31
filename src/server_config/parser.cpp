/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 16:19:19 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/31 18:11:08 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

// value & flag (uses bit and comparison)
constexpr bool Config::has_flag(ConfigContext value, ConfigContext flag)
{
    return (static_cast<uint>(value) & static_cast<uint>(flag));
}

// gets on token and move head, returns at the end eof if it is a  second time callet i thros a error
Token Config::get_next_token(std::vector<Token>::const_iterator &head_token)
{
	Token current_token = *head_token;

	if (current_token.type == TokenType::END_OF_FILE)
		throw ConfigParseException("unexpected end of file", current_token);
	++head_token;
    return current_token;
}

// gets on token and move head
Token Config::get_next_word(std::vector<Token>::const_iterator &head_token)
{
	Token token = get_next_token(head_token);

	if (token.type != TokenType::WORD)
		throw ConfigParseException("expected a word", token);
	
    return token;
}

void Config::skip_next(std::vector<Token>::const_iterator &head_token, TokenType expected_type)
{
	Token token = get_next_token(head_token);

	if (token.type != expected_type)
		throw ConfigParseException("expected a different token type", token);

	return;
}

DirectiveLookup Config::find_directive_lookup(const Token & token, ConfigContext context)
{
	if (token.type != TokenType::WORD)
		throw ConfigParseException("expected a word", token);
	for (auto lookup : directive_lookup)
	{
		if (has_flag(lookup.allowed_contexts, context) && 
				lookup.word == token.word)
			return lookup;
	}
	throw ConfigParseException("unknown word, not found in list", token);
}






void Config::fill_location_context(TokenIterator &head_token,
	std::vector<LocationConfig> &fill_config)
{
	skip_next(head_token, TokenType::OPEN_BRACE);

	LocationConfig new_conf;

	while (true)
	{
		Token current_token = get_next_token(head_token);
		if (current_token.type == TokenType::CLOSE_BRACE)
			break;
		
		DirectiveLookup current_lookup = find_directive_lookup(current_token, ConfigContext::LOCATION);

		if (current_lookup.fill_context == nullptr)
			throw ConfigParseException("programming error : 201", current_token);
		else
			(this->*current_lookup.fill_context)(head_token, new_conf, current_lookup);

	}

	fill_config.push_back(new_conf);
	return;
	
}



void Config::fill_server_context(TokenIterator &head_token)
{
	skip_next(head_token, TokenType::OPEN_BRACE);

	ServerConfig new_conf;

	while (true)
	{
		Token current_token = get_next_token(head_token);
		if (current_token.type == TokenType::CLOSE_BRACE)
			break;

		DirectiveLookup current_lookup = find_directive_lookup(current_token, ConfigContext::SERVER);
		
		if (current_lookup.fill_context == nullptr)
			throw ConfigParseException("programming error : 201", current_token);
		else
			(this->*current_lookup.fill_context)(head_token, new_conf, current_lookup);

	}
	server_confs.push_back(new_conf);
	return;
}


void Config::fill_http_context(TokenIterator &head_token)
{
	skip_next(head_token, TokenType::OPEN_BRACE);

	while (true)
	{
		Token current_token = get_next_token(head_token);
		if (current_token.type == TokenType::CLOSE_BRACE)
			break;

		DirectiveLookup current_lookup = find_directive_lookup(current_token, ConfigContext::HTTP);
		
		if (current_lookup.fill_context == nullptr)
			fill_server_context(head_token);
		else
			(this->*current_lookup.fill_context)(head_token, http_conf, current_lookup);
	}
	return;
}


void Config::fill_main_context(TokenIterator &head_token)
{
	Token current_token = get_next_word(head_token);
	find_directive_lookup(current_token, ConfigContext::MAIN);

	fill_http_context(head_token);
	skip_next(head_token, TokenType::END_OF_FILE);
	
	return;
}





























void Config::fill_error_page_context(
			TokenIterator &head_token,
			DefaultConfig &fill_config,
			const DirectiveLookup &current_lookup)
{
	(void)current_lookup;
	// LOG(LOG_DEBUG, "NOICE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

	Token current_token = get_next_word(head_token);
	while (current_token.type == TokenType::WORD)
	{
		try
		{
			fill_config.error_codes.push_back(std::stoi(current_token.word));
		}
		catch(const std::exception& e)
		{
			try
			{
				fill_config.error_page_path = current_token.word;
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		
		current_token = get_next_word(head_token);
	}
	
	
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void Config::fill_client_max_body_size_context(TokenIterator &head_token,
					DefaultConfig &fill_config,
					const DirectiveLookup &current_lookup)
{
	(void)current_lookup;
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.client_max_body_size = std::stoul(current_token.word);
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void Config::fill_autoindex_context(TokenIterator &head_token,
					DefaultConfig &fill_config,
					const DirectiveLookup &current_lookup)
{
	(void)current_lookup;
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.autoindex = (current_token.word == "on");
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void Config::fill_root_context(TokenIterator &head_token,
					DefaultConfig &fill_config,
					const DirectiveLookup &current_lookup)
{
	(void)current_lookup;
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.root = current_token.word;
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void Config::fill_index_context(TokenIterator &head_token,
					DefaultConfig &fill_config,
					const DirectiveLookup &current_lookup)
{
	(void)current_lookup;
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.index.push_back(current_token.word);
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void Config::fill_listen_context(TokenIterator &head_token,
					DefaultConfig &fill_config,
					const DirectiveLookup &current_lookup)
{
	(void)current_lookup;
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.listen = current_token.word;
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void Config::fill_server_name_context(TokenIterator &head_token,
					DefaultConfig &fill_config,
					const DirectiveLookup &current_lookup)
{
	(void)current_lookup;
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.server_name = current_token.word;
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void Config::fill_limit_except_context(TokenIterator &head_token,
					DefaultConfig &fill_config,
					const DirectiveLookup &current_lookup)
{
	(void)current_lookup;
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.limit_except = current_token.word;
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void Config::fill_return_context(TokenIterator &head_token,
					DefaultConfig &fill_config,
					const DirectiveLookup &current_lookup)
{
	(void)current_lookup;
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.return = current_token.word;
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}