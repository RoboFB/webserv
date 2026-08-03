/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printing.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 12:03:20 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/31 18:46:07 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "Config.hpp"

// printing functions
void print_parsing_error(const Token &token, const std::string &filename);

void debug_print_tokens(const std::vector<Token> &tokens);
void debug_print_config(const MainConfig &config);

std::ostream &operator<<(std::ostream &os, const TokenType &token);
std::ostream &operator<<(std::ostream &os, const Token &token);
std::ostream &operator<<(std::ostream &os, const struct addrinfo *servinfo);
std::ostream &operator<<(std::ostream &os, const struct addrinfo &servinfo);

std::ostream &operator<<(std::ostream &os, const DefaultConfig &config);
std::ostream &operator<<(std::ostream &os, const HttpConfig &config);
std::ostream &operator<<(std::ostream &os, const MainConfig &config);