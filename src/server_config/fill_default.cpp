/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_default.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 18:38:24 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/12 17:35:51 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <algorithm>
#include <cstring>

#include "printing.hpp"


void Config::append_addrinfo(AddrInfoPtr &addr_list, const std::string &address_str, const std::string &port_str, const Token &token)
{
	addrinfo hints{};
	hints.ai_flags = AI_PASSIVE;		// fill in my IP for me
	hints.ai_family = AF_UNSPEC;		// don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets

	addrinfo* new_list = nullptr;
	
	int status = getaddrinfo(address_str.c_str(), port_str.c_str(), &hints, &new_list);
	if (status != 0)
		throw ConfigParseException("getaddrinfo error: " + std::string(gai_strerror(status)), token);
	
	if (addr_list.get() == nullptr)
	{
		addr_list.reset(new_list);
	}
	else
	{
		addrinfo* current = addr_list.get();
		while (current->ai_next)
			current = current->ai_next;
		current->ai_next = new_list;
	}
}


/* 
listen port
listen ip:port

listen 127.0.0.1:8000;
listen 127.0.0.1;
listen 8000;

ipv6 not supported yet, but should be in the future

default: 0.0.0.0:8000; and 0.0.0.0:80;
*/
void Config::fill_listen(TokenIterator &head_token,
					DefaultConfig &fill_config)
{
	Token token = get_next_word(head_token);

	std::string port_str(token.word);
	std::string address_str = "0.0.0.0";// default

	size_t count_colon = std::count(port_str.begin(), port_str.end(), ':');
	if (count_colon > 1)
	{
		throw ConfigParseException("invalid listen format to many colons", token);
    }
	else if (count_colon == 1)
	{
		std::getline(std::istringstream(port_str), address_str, ':');
		port_str = port_str.substr(address_str.length() + 1);
    }
	if (validate_port(port_str) == -1)
		throw ConfigParseException("invalid port number", token);
	
	
	append_addrinfo(reinterpret_cast<ServerConfig&>(fill_config).listen, address_str, port_str, token);


	
	// debug print
	std::cout << "address_str:" << address_str << "%" << std::endl;
	std::cout << "port_str:" << port_str << "%" << std::endl;
	std::cout << "server_conf->listen:" << *reinterpret_cast<ServerConfig&>(fill_config).listen << std::endl;

	skip_next(head_token, TokenType::SEMICOLON);
	return;
}





/* 
default: off
*/
void Config::fill_autoindex(TokenIterator &head_token,
					DefaultConfig &fill_config)
{
	Token token = get_next_word(head_token);

	if (token.word == "on")
	{
		fill_config.autoindex = true;
	}
	else if (token.word == "off")
	{
		fill_config.autoindex = false;
	}
	else
	{
		throw ConfigParseException("invalid autoindex value, expected 'on' or 'off'", token);
	}

	skip_next(head_token, TokenType::SEMICOLON);
	return;
}



/* 
Default: root html;
*/
void Config::fill_root(TokenIterator &head_token,
					DefaultConfig &fill_config)
{
	Token token = get_next_word(head_token);

	fill_config.root_path = token.word;
	// todo: maybe more error handling


	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

/* 
converts "3G" "20M" "1024k" "10" in Bytes (size_t)
*/
size_t Config::parse_max_body_size(std::string number_str, const Token &token)
{
	size_t x = number_str.rfind("iB");
	if (x != std::string::npos)
		number_str.erase(x, 2);

    char unit_letter = number_str.back();
	
	size_t multiplier = 1;
	if (!std::isdigit(unit_letter))
	{
		number_str.pop_back();
		if (number_str.length() == 0)
			throw ConfigParseException("invalid client_max_body_size value, no number before unit", token);
		switch (std::tolower(unit_letter)) {
			case 'b': multiplier = BODY_SIZE_B; break;
			case 'k': multiplier = BODY_SIZE_K; break;
			case 'm': multiplier = BODY_SIZE_M; break;
			case 'g': multiplier = BODY_SIZE_G; break;
			default: throw ConfigParseException("wrong modifier client_max_body_size value", token);
		}
	}


	size_t index = 0;
	int result;
	try {
		result = std::stoi(number_str, &index);
	} 
	catch (const std::exception& e) {
		throw ConfigParseException("invalid number", token);
	}
	if (index != number_str.length())
		throw ConfigParseException("invalid number" , token);
	else if (result < 0)
		throw ConfigParseException("invalid negative number", token);
	else if (SIZE_MAX / multiplier < static_cast<size_t>(result))
		throw ConfigParseException("invalid big number", token);
	
	return result * multiplier;	
}

/* 
Default:	
client_max_body_size 1m
*/
void Config::fill_client_max_body_size(TokenIterator &head_token,
					DefaultConfig &fill_config)
{
	Token token = get_next_word(head_token);
	fill_config.client_max_body_size = parse_max_body_size(token.word, token);
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}












void Config::fill_index(TokenIterator &head_token,
					DefaultConfig &fill_config)
{
	Token token = get_next_word(head_token);

	// todo: maybe more error handling
	fill_config.indexs_paths.push_back(token.word);

	skip_next(head_token, TokenType::SEMICOLON);
	return;
}

// void Config::fill_server_name(TokenIterator &head_token,
// 					DefaultConfig &fill_config)
// {
// 	Token token = get_next_word(head_token);

// 	if (token.word.empty()) // todo: maybe more error handling
// 		throw ConfigParseException("empty server name", token);
// 	fill_config.server_names.push_back(token.word);

// 	skip_next(head_token, TokenType::SEMICOLON);
// 	return;
// }



AllowedMethods string_to_get_allowed_methods(const std::string& methods)
{
	if (methods == "GET")
		return AllowedMethods::GET;
	else if (methods == "POST")
		return AllowedMethods::POST;
	else if (methods == "DELETE")
		return AllowedMethods::DELETE;
	else
		return AllowedMethods::NONE;
}

std::string allowed_methods_to_string(AllowedMethods methods)
{
	if (methods == AllowedMethods::GET)
		return "GET";
	else if (methods == AllowedMethods::POST)
		return "POST";
	else if (methods == AllowedMethods::DELETE)
		return "DELETE";
	else if (methods == AllowedMethods::NONE)
		return "NONE";
	else
		return "NONE";
}


/* 

default: limit_except NONE; Breaking to nginx but the easiest to implement and make a little sense. maby remane it to allowd methods.
        so the config needa at least one methet. 
		can be difficult to inhert it kouse it only adds and not overwrite values prevuslie set.
limit_except GET POST DELETE;
limit_except GET;
limit_except POST;
limit_except DELETE;

*/
void Config::fill_limit_except(TokenIterator &head_token,
					DefaultConfig &fill_config)
{
	Token token = get_next_word(head_token);
	while (true)
	{
		AllowedMethods method = string_to_get_allowed_methods(token.word);
		if (method == AllowedMethods::NONE)
			throw ConfigParseException("invalid limit_except needs 'GET', 'POST' or 'DELETE'", token);
		fill_config.limit_except = fill_config.limit_except | method;

		token = get_next_token(head_token);
		if (token.type == TokenType::SEMICOLON)
			break;
		if (token.type != TokenType::WORD)
			throw ConfigParseException("expected a word", token);
	}
	return;
}



/* 
BIG TODO:
- fix and implement all the fill_context functions for the directives

*/


// TODO: implement return directive
void Config::fill_return(TokenIterator &head_token,
					DefaultConfig &fill_config)
{
	Token token = get_next_word(head_token);

	fill_config.return_code = std::stoi(token.word);

	token = get_next_token(head_token); // get next token, should be a word or a semicolon

	if (token.type == TokenType::WORD)
	{
		fill_config.return_path = token.word;
		skip_next(head_token, TokenType::SEMICOLON);
		return;
	}
	else if (token.type == TokenType::SEMICOLON)
	{
		return;
	}
	else 
		throw ConfigParseException("expected a word or a semicolon", token);

	return;
}




// TODO: implement error_page directive
void Config::fill_error_page(
			TokenIterator &head_token,
			DefaultConfig &fill_config)
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
			fill_config.error_page_path = current_token.word;
		}
		
		current_token = get_next_word(head_token);
	}
	
	
	skip_next(head_token, TokenType::SEMICOLON);
	return;
}