/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_context.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 18:43:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 13:08:26 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

void ConfigParser::fill_location(TokenIterator &head_token,
								 std::vector<LocationConfigParser> &fill_config)
{
	LocationConfigParser new_conf;

	new_conf.location_path = get_next_word(head_token).word;

	skip_next(head_token, TokenType::OPEN_BRACE);

	while (true)
	{
		Token current_token = get_next_token(head_token);
		if (current_token.type == TokenType::CLOSE_BRACE)
			break;

		DirectiveLookup current_lookup =
			find_directive_lookup(current_token, ConfigContext::LOCATION);

		if (current_lookup.fill_context == nullptr)
			throw ConfigParserException("programming error : 201",
										current_token);
		else
			(*current_lookup.fill_context)(head_token, new_conf);
	}

	fill_config.push_back(new_conf);
	return;
}

void ConfigParser::fill_server(TokenIterator &head_token,
							   std::vector<ServerConfigParser> &fill_config)
{
	skip_next(head_token, TokenType::OPEN_BRACE);

	ServerConfigParser new_conf;

	while (true)
	{
		Token current_token = get_next_token(head_token);
		if (current_token.type == TokenType::CLOSE_BRACE)
			break;

		DirectiveLookup current_lookup =
			find_directive_lookup(current_token, ConfigContext::SERVER);

		if (current_lookup.fill_context == nullptr)
			fill_location(head_token, new_conf.locations_confs);
		else
			(*current_lookup.fill_context)(head_token, new_conf);
	}
	fill_config.push_back(std::move(new_conf));
	return;
}

void ConfigParser::fill_http(TokenIterator &head_token,
							 HttpConfigParser &fill_config)
{
	skip_next(head_token, TokenType::OPEN_BRACE);

	while (true)
	{
		Token current_token = get_next_token(head_token);
		if (current_token.type == TokenType::CLOSE_BRACE)
			break;

		DirectiveLookup current_lookup =
			find_directive_lookup(current_token, ConfigContext::HTTP);

		if (current_lookup.fill_context == nullptr)
			fill_server(head_token, fill_config.servers_confs);
		else
			(*current_lookup.fill_context)(head_token, fill_config);
	}
	return;
}

void ConfigParser::fill_main(TokenIterator &head_token,
							 MainConfigParser &fill_config)
{
	Token current_token = get_next_word(head_token);
	find_directive_lookup(current_token, ConfigContext::MAIN);

	fill_http(head_token, fill_config.http_conf);
	is_at_end_of_file(head_token);

	return;
}
