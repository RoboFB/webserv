/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_default.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 18:38:24 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/31 18:46:07 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"


/* 
BIG TODO:
- fix and implement all the fill_context functions for the directives

*/

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