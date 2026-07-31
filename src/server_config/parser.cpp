/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 16:19:19 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/31 12:06:18 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"


constexpr bool has_flag(ConfigContext value, ConfigContext flag)
{
    return (static_cast<uint>(value) & static_cast<uint>(flag));
}

// gets on token and move head, returns at the end eof if it is a  second time callet i thros a error
Token get_next_token(std::vector<Token>::const_iterator &head_token)
{
	Token current_token = *head_token;

	if (current_token.type == TokenType::END_OF_FILE)
		throw ConfigParseException("unexpected end of file", current_token);
	++head_token;
    return current_token;
}

// gets on token and move head
Token get_next_word(std::vector<Token>::const_iterator &head_token)
{
	Token token = get_next_token(head_token);

	if (token.type != TokenType::WORD)
		throw ConfigParseException("expected a word", token);
	
    return token;
}

void skip_next(std::vector<Token>::const_iterator &head_token, TokenType expected_type)
{
	Token token = get_next_token(head_token);

	if (token.type != expected_type)
		throw ConfigParseException("expected a different token type", token);

	return;
}

// Token check_word(const Token & token, const std::string & expected_word)
// {
// 	if (token.type != TokenType::WORD)
// 		throw ConfigParseException("expected a word", token);
// 	if (token.word != expected_word)
// 		throw ConfigParseException("expected '" + expected_word + "'", token);
//     return token;
// }

DirectiveLookup get_directive_lookup(const Token & token, ConfigContext context)
{
	if (token.type != TokenType::WORD)
		throw ConfigParseException("expected a word", token);
	for (auto lookup : g_directive_lookup)
	{
		if (has_flag(lookup.allowed_contexts, context) && 
				lookup.word == token.word)
			return lookup;
	}
	throw ConfigParseException("unknown word, not found in list", token);
}





// template <typename TContext>
// void fill_context(	TContext &fill_config,
// 					ConfigContext fill_context,
// 					std::vector<Token>::const_iterator head_token) 
// {
// 	Token current_token = get_next_word(head_token);
// 	auto current_lookup = get_directive_lookup(current_token, fill_context);

// 	// i have the correct line out of the lookup thats valid for the curent context

// 	if (current_lookup.terminator == TokenType::OPEN_BRACE)
// 	{
// 		skip_next(head_token, TokenType::OPEN_BRACE);

// 		fill_config.
// 		fill_context<current_lookup>(fill_config, current_lookup, head_token);

// 		skip_next(head_token, TokenType::CLOSE_BRACE);
// 	}
// 	else if (current_lookup.terminator == TokenType::SEMICOLON)
// 	{
// 		fill_context<current_lookup>(fill_config, current_lookup, head_token);
		
// 		skip_next(head_token, TokenType::SEMICOLON);
// 	}
// 	else
// 		throw ConfigParseException("unknown terminator type", current_token);
// 	return;
// }


void fill_error_page_context(DefaultContext &fill_config,
					std::vector<Token>::const_iterator &head_token)
{
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

void fill_client_max_body_size_context(DefaultContext &fill_config,
					std::vector<Token>::const_iterator &head_token)
{
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.client_max_body_size = std::stoul(current_token.word);
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void fill_autoindex_context(DefaultContext &fill_config,
					std::vector<Token>::const_iterator &head_token)
{
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.autoindex = (current_token.word == "on");
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void fill_root_context(DefaultContext &fill_config,
					std::vector<Token>::const_iterator &head_token)
{
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.root = current_token.word;
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void fill_index_context(DefaultContext &fill_config,
					std::vector<Token>::const_iterator &head_token)
{
	Token current_token = get_next_word(head_token);
	(void)current_token; (void)fill_config;
	// fill_config.index.push_back(current_token.word);
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

void fill_default_context(DefaultContext &fill_config,
					std::vector<Token>::const_iterator &head_token,
					const DirectiveLookup &current_lookup)
{
	if (current_lookup.word == "error_page")
		fill_error_page_context(fill_config, head_token);
	else if (current_lookup.word == "client_max_body_size")
		fill_client_max_body_size_context(fill_config, head_token);
	else if (current_lookup.word == "autoindex")
		fill_autoindex_context(fill_config, head_token);
	else if (current_lookup.word == "root")
		fill_root_context(fill_config, head_token);
	else if (current_lookup.word == "index")
		fill_index_context(fill_config, head_token);
	else
		throw ConfigParseException("unknown word, not found in list 44");
	return;
}


void fill_location_context(MainContext &fill_config,
					std::vector<Token>::const_iterator &head_token)
{
	Token current_token = get_next_word(head_token);
	auto current_lookup = get_directive_lookup(current_token, ConfigContext::LOCATION);

	if (current_lookup.terminator == TokenType::OPEN_BRACE)
	{
		skip_next(head_token, TokenType::OPEN_BRACE);
		(void)fill_config;
		// fill_location_context(fill_config, ConfigContext::LOCATION, head_token);
		skip_next(head_token, TokenType::CLOSE_BRACE);
	}
	else
	{
		if (current_lookup.allowed_contexts == ConfigContext::DEFAULT)
		{
			fill_default_context(dynamic_cast<DefaultContext&>(fill_config.http_context.server_context.back().location_context.back()), 
					head_token, current_lookup);
		}
	}
	return;
}


void fill_server_context(MainContext &fill_config,
					std::vector<Token>::const_iterator &head_token)
{
	Token current_token = get_next_word(head_token);
	auto current_lookup = get_directive_lookup(current_token, ConfigContext::SERVER);

	if (current_lookup.terminator == TokenType::OPEN_BRACE)
	{
		skip_next(head_token, TokenType::OPEN_BRACE);
		fill_location_context(fill_config, head_token);
		skip_next(head_token, TokenType::CLOSE_BRACE);
	}
	else
	{
		if (current_lookup.allowed_contexts == ConfigContext::DEFAULT)
		{
			fill_default_context(dynamic_cast<DefaultContext&>(fill_config.http_context.server_context.back()), 
					head_token, current_lookup);
		}
	}
	return;
}


void fill_http_context(MainContext &fill_config,
					std::vector<Token>::const_iterator &head_token)
{
	Token current_token = get_next_word(head_token);
	DirectiveLookup current_lookup = get_directive_lookup(current_token, ConfigContext::HTTP);

	if (current_lookup.terminator == TokenType::OPEN_BRACE)
	{
		skip_next(head_token, TokenType::OPEN_BRACE);
		fill_server_context(fill_config, head_token);
		skip_next(head_token, TokenType::CLOSE_BRACE);
	}
	else
	{
		
		if (current_lookup.allowed_contexts == ConfigContext::DEFAULT)
		{
			fill_default_context(dynamic_cast<DefaultContext&>(fill_config.http_context), 
					head_token, current_lookup);
		}
	}
	return;
}

void fill_main_context(	MainContext &fill_config,
					std::vector<Token>::const_iterator &head_token)
{
	Token current_token = get_next_word(head_token);
	auto current_lookup = get_directive_lookup(current_token, ConfigContext::MAIN);

	if (current_lookup.terminator == TokenType::OPEN_BRACE)
	{
		skip_next(head_token, TokenType::OPEN_BRACE);
		fill_http_context(fill_config, head_token);
		skip_next(head_token, TokenType::CLOSE_BRACE);
	}
	else
		throw ConfigParseException("unknown terminator type 4", current_token);
	return;
}
