/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_default.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 18:38:24 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/06 15:07:43 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <algorithm>
#include <cstring>

#include "printing.hpp"

/* 
BIG TODO:
- fix and implement all the fill_context functions for the directives

*/


/* 
listen port
listen ip:port

listen 127.0.0.1:8000;
listen 127.0.0.1;
listen 8000;

ipv6 not supported yet, but should be in the future

default: 0.0.0.0:8000; and 0.0.0.0:80;
*/
void Config::fill_listen_context(TokenIterator &head_token,
					DefaultConfig &fill_config,
					const DirectiveLookup &current_lookup)
{
	(void)current_lookup;
	Token token = get_next_word(head_token);
	std::string port_str(token.word);
	
	
	std::string address_str = "0.0.0.0";// default
	if (std::count(port_str.begin(), port_str.end(), ':') == 1)
	{
		std::getline(std::istringstream(port_str), address_str, ':');
		port_str = port_str.substr(address_str.length() + 1);
    }

	if (validate_port(port_str) == -1)
		throw ConfigParseException("invalid port number", token);
	
	

	ServerConfig *server_conf = static_cast<ServerConfig *>(&fill_config);

	int status;
	addrinfo hints{};
	hints.ai_flags = AI_PASSIVE;		// fill in my IP for me
	hints.ai_family = AF_UNSPEC;		// don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets

	
	addrinfo* new_list = nullptr;
	status = getaddrinfo(address_str.c_str(), port_str.c_str(), &hints, &new_list);

	if (status != 0)
	{
		throw ConfigParseException("getaddrinfo error: " + std::string(gai_strerror(status)), token);
	}
	if (server_conf->listen == nullptr)
	{
		server_conf->listen.reset(new_list);
	}
	else
	{
		addrinfo* current = server_conf->listen.get();
		while (current->ai_next)
			current = current->ai_next;
		current->ai_next = new_list;
	}
	
	// debug print
	std::cout << "address_str:" << address_str << "%" << std::endl;
	std::cout << "port_str:" << port_str << "%" << std::endl;
	std::cout << "server_conf->listen:" << *server_conf->listen << std::endl;

	skip_next(head_token, TokenType::SEMICOLON);
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