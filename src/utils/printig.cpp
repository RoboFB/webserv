/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printig.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 13:59:22 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/19 18:03:34 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printing.hpp"
#include "Config.hpp"
#include "logging.hpp"
#include <netdb.h>

/* 
	mostly ai generated
*/

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



static std::string make_indent(size_t level)
{
	return std::string(level * 4, ' ');
}

// space separated "host:port" list, one listen directive worth of addresses
static std::string listen_to_string(const AddrInfoPtr &listen)
{
	std::string result;

	for (const addrinfo *current = listen.get(); current != nullptr; current = current->ai_next)
	{
		if (!result.empty())
			result += ' ';
		result += addr_to_string(current);
	}
	return result;
}

static void print_root(std::ostream &os, const DefaultConfig &config, size_t indent)
{
	if (config.root_path)
		os << make_indent(indent) << "root " << config.root_path->string() << ";\n";
}

static void print_index(std::ostream &os, const DefaultConfig &config, size_t indent)
{
	if (config.indexs_paths.empty())
		return;
	os << make_indent(indent) << "index";
	for (const auto &index_path : config.indexs_paths)
		os << ' ' << index_path.string();
	os << ";\n";
}

static void print_autoindex(std::ostream &os, const DefaultConfig &config, size_t indent)
{
	if (config.autoindex)
	{
		if (config.autoindex.value())
			os << make_indent(indent) << "autoindex on;\n";
		else
			os << make_indent(indent) << "autoindex off;\n";
	}
	else
		os << make_indent(indent) << "autoindex not set;\n";
}

static void print_client_max_body_size(std::ostream &os, const DefaultConfig &config, size_t indent)
{
	if (config.client_max_body_size)
		os << make_indent(indent) << "client_max_body_size " << config.client_max_body_size.value() << ";\n";
}

static void print_limit_except(std::ostream &os, const DefaultConfig &config, size_t indent)
{
	if (config.limit_except != Methods::NONE)
		os << make_indent(indent) << "limit_except " << methods_to_string(config.limit_except) << ";\n";
}

static void print_error_page(std::ostream &os, const DefaultConfig &config, size_t indent)
{
	if (config.error_codes.empty() && !config.error_page_path)
		return;
	os << make_indent(indent) << "error_page";
	for (int code : config.error_codes)
		os << ' ' << code;
	if (config.error_page_path)
		os << ' ' << config.error_page_path->string();
	os << ";\n";
}

static void print_return(std::ostream &os, const DefaultConfig &config, size_t indent)
{
	if (!config.return_code && !config.return_path)
		return;
	os << make_indent(indent) << "return " << config.return_code.value_or(0);
	if (config.return_path)
		os << ' ' << config.return_path->string();
	os << ";\n";
}

// picks the print function by the directive's own name out of directive_lookup, so the set of
// directive names lives in exactly one place (the parser's table) instead of being repeated here
static void print_directive_by_name(std::ostream &os, const DefaultConfig &config, size_t indent, std::string_view name)
{
	if (name == "root")
		print_root(os, config, indent);
	else if (name == "index")
		print_index(os, config, indent);
	else if (name == "autoindex")
		print_autoindex(os, config, indent);
	else if (name == "client_max_body_size")
		print_client_max_body_size(os, config, indent);
	else if (name == "limit_except")
		print_limit_except(os, config, indent);
	else if (name == "error_page")
		print_error_page(os, config, indent);
	else if (name == "return")
		print_return(os, config, indent);
}

void Config::print_default_directives(std::ostream &os, const DefaultConfig &config, size_t indent)
{
	for (const auto &entry : directive_lookup)
		print_directive_by_name(os, config, indent, entry.word);
}

static void print_location(std::ostream &os, const LocationConfig &config, size_t indent)
{
	const std::string pad = make_indent(indent);

	os << pad << "location " << config.location_path.string() << " {\n";
	Config::print_default_directives(os, config, indent + 1);
	os << pad << "}\n";
}

static void print_server(std::ostream &os, const ServerConfig &config, size_t indent)
{
	const std::string pad = make_indent(indent);

	os << pad << "server {\n"
	   << make_indent(indent + 1) << "listen " << listen_to_string(config.listen) << ";\n";
	Config::print_default_directives(os, config, indent + 1);
	for (const auto &location : config.locations_confs)
	{
		os << '\n';
		print_location(os, location, indent + 1);
	}
	os << pad << "}\n";
}

static void print_http(std::ostream &os, const HttpConfig &config, size_t indent)
{
	const std::string pad = make_indent(indent);

	os << pad << "http {\n";
	Config::print_default_directives(os, config, indent + 1);
	for (const auto &server : config.servers_confs)
	{
		os << '\n';
		print_server(os, server, indent + 1);
	}
	os << pad << "}\n";
}

std::ostream &operator<<(std::ostream &os, const HttpConfig &config)
{
	print_http(os, config, 0);
	return os;
}

std::ostream &operator<<(std::ostream &os, const MainConfig &config)
{
	return os << config.http_conf;
}

std::ostream &operator<<(std::ostream &os, const Config &config)
{
	return os << config.to_string();
}
