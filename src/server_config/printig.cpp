/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printig.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 13:59:22 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/31 12:07:08 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printing.hpp"
#include "config.hpp"
#include "logging.hpp"
#include <netdb.h>
#include <arpa/inet.h>

namespace
{

const char *addr_family_to_string(int family)
{
	switch (family)
	{
	case AF_UNSPEC: return "AF_UNSPEC";
	case AF_INET: return "AF_INET";
	case AF_INET6: return "AF_INET6";
	default: return "AF_UNKNOWN";
	}
}

const char *socket_type_to_string(int socktype)
{
	switch (socktype)
	{
	case 0: return "ANY";
	case SOCK_STREAM: return "SOCK_STREAM";
	case SOCK_DGRAM: return "SOCK_DGRAM";
	case SOCK_RAW: return "SOCK_RAW";
	default: return "SOCK_UNKNOWN";
	}
}

std::string addr_to_string(const struct addrinfo *info)
{
	if (!info || !info->ai_addr)
		return "(none)";

	char host[NI_MAXHOST];
	char service[NI_MAXSERV];
	int result = getnameinfo(info->ai_addr, info->ai_addrlen,
		host, sizeof(host), service, sizeof(service),
		NI_NUMERICHOST | NI_NUMERICSERV);
	if (result != 0)
		return gai_strerror(result);

	return std::string(host) + ":" + service;
}

}

void print_parsing_error(const Token &token, const std::string &filename)
{
	std::stringstream error_message;

	error_message << filename
		<< ":"
		<< token.line
		<< ":"
		<< token.column
		<< ": parsing error at: '"
		<< token.word
		<< "'\n";

	LOG(LOG_ERROR, error_message.str());
}

// print all tokens for debugging
void debug_print_tokens(const std::vector<Token> &tokens)
{
	if (!(LOG_LVL & LOG_DEBUG))
		return;
	std::stringstream log_all_tokens;

	for (const auto &token : tokens)
	{
		log_all_tokens << token << std::endl;
	}
	LOG(LOG_DEBUG, log_all_tokens.str());
}

// print complete config for debugging
void debug_print_config(const MainContext &config)
{
	if (!(LOG_LVL & LOG_DEBUG))
		return;
	std::stringstream config_stream;
	config_stream << config;
	LOG(LOG_DEBUG, config_stream.str());
}

std::ostream &operator<<(std::ostream &os, const TokenType &token)
{
	switch (token)
	{
	case TokenType::WORD:			os << "WORD       "; return os;
	case TokenType::OPEN_BRACE:		os << "OPEN_BRACE "; return os;
	case TokenType::CLOSE_BRACE:	os << "CLOSE_BRACE"; return os;
	case TokenType::SEMICOLON:		os << "SEMICOLON  "; return os;
	default: 						os << "UNKNOWN    "; return os;
	}
}

std::ostream &operator<<(std::ostream &os, const Token &token)
{
	os	<< token.type
		<< " "
    	<< token.line
        << ":"
        << token.column
		<< " \t"
		<< token.word;
		return os;
}


std::ostream &operator<<(std::ostream &os, const struct addrinfo *servinfo)
{
	if (!servinfo)
		return os << "addrinfo: null";

	return os << *servinfo;
}

std::ostream &operator<<(std::ostream &os, const struct addrinfo &servinfo)

{
	os << "addrinfo:";
	for (const struct addrinfo *current = &servinfo; current != nullptr; current = current->ai_next)
	{
		os << '\n'
		   << "  family:    " << addr_family_to_string(current->ai_family)
		   << '\n'
		   << "  socktype:  " << socket_type_to_string(current->ai_socktype)
		   << '\n'
		   << "  protocol:  " << current->ai_protocol
		   << '\n'
		   << "  flags:     " << current->ai_flags
		   << '\n'
		   << "  addrlen:   " << current->ai_addrlen
		   << '\n'
		   << "  address:   " << addr_to_string(current)
		   << '\n'
		   << "  canonname: ";
		if (current->ai_canonname)
			os << current->ai_canonname;
		else
			os << "null";
		if (current->ai_next)
			os << '\n' << "  next:      more entries follow:";
		else
			os << '\n' << "  next:      end";
	}
	return os;
}



// TODO: implement printing for all contexts of the config completely at the moment the acutal data is missing.
std::ostream &operator<<(std::ostream &os, const DefaultContext &config)
{
	return os << "DefaultContext: "
		<< "error page with " << config.error_codes.size() << " error codes, ";
}

std::ostream &operator<<(std::ostream &os, const HttpContext &config)
{
	return os << "HttpContext: "
		<< dynamic_cast<const DefaultContext&>(config);
}

std::ostream &operator<<(std::ostream &os, const MainContext &config)
{
	return os << "MainContext: "
		<< config.http_context;
}




