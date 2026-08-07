/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_context.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 18:43:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/07 20:28:33 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"




void Config::fill_location(TokenIterator &head_token,
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
			(this->*current_lookup.fill_context)(head_token, new_conf);

	}

	fill_config.push_back(new_conf);
	return;
	
}



void Config::fill_server(TokenIterator &head_token)
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
			(this->*current_lookup.fill_context)(head_token, new_conf);

	}
	server_confs.push_back(std::move(new_conf));
	return;
}


void Config::fill_http(TokenIterator &head_token)
{
	skip_next(head_token, TokenType::OPEN_BRACE);

	while (true)
	{
		Token current_token = get_next_token(head_token);
		if (current_token.type == TokenType::CLOSE_BRACE)
			break;

		DirectiveLookup current_lookup = find_directive_lookup(current_token, ConfigContext::HTTP);
		
		if (current_lookup.fill_context == nullptr)
			fill_server(head_token);
		else
			(this->*current_lookup.fill_context)(head_token, http_conf);
	}
	return;
}


void Config::fill_main(TokenIterator &head_token)
{
	Token current_token = get_next_word(head_token);
	find_directive_lookup(current_token, ConfigContext::MAIN);

	fill_http(head_token);
	is_at_end_of_file(head_token);

	return;
}
