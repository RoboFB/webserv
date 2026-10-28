/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printing.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 12:03:20 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/28 19:18:32 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"

/*
	mostly ai generated
*/

// printing functions
void debug_print_tokens(const std::vector<Token> &tokens);

std::ostream &operator<<(std::ostream &os, const TokenType &token);
std::ostream &operator<<(std::ostream &os, const Token &token);

std::ostream &operator<<(std::ostream &os, const HttpConfigPars &config);
std::ostream &operator<<(std::ostream &os, const MainConfigPars &config);