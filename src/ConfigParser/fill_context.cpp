/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_context.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 18:43:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 18:53:38 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

void ConfigParser::fill_location(TokenIterator &head_token,
								 LocationConfigParser &fill_location)
{
	fill_location.location_path = get_next_word(head_token).word;

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
			(*current_lookup.fill_context)(head_token, fill_location);
	}
	return;
}

void ConfigParser::fill_server(TokenIterator &head_token,
							   ServerConfigParser &fill_server)
{
	skip_next(head_token, TokenType::OPEN_BRACE);
	while (true)
	{
		Token current_token = get_next_token(head_token);
		if (current_token.type == TokenType::CLOSE_BRACE)
			break;

		DirectiveLookup current_lookup =
			find_directive_lookup(current_token, ConfigContext::SERVER);

		if (current_lookup.fill_context == nullptr)
			fill_location(head_token,
						  fill_server.locations_confs.emplace_back());
		else
			(*current_lookup.fill_context)(head_token, fill_server);
	}

	if (fill_server.locations_confs.empty())
		throw ConfigParserException("missing location block in server block",
									*(head_token - 1));
	return;
}

void ConfigParser::fill_http(TokenIterator &head_token,
							 HttpConfigParser &fill_http)
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
			fill_server(head_token, fill_http.servers_confs.emplace_back());
		else
			(*current_lookup.fill_context)(head_token, fill_http);
	}
	if (fill_http.servers_confs.empty())
		throw ConfigParserException("missing server block in http block",
									*(head_token - 1));
	return;
}

void ConfigParser::fill_main(TokenIterator &head_token,
							 MainConfigParser &main_config)
{
	Token current_token = get_next_word(head_token);
	find_directive_lookup(current_token, ConfigContext::MAIN);

	fill_http(head_token, main_config.http_conf);

	is_at_end_of_file(head_token);

	return;
}
