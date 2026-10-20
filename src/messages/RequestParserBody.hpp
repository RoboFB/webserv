/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserBody.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 20:54:35 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 22:32:42 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "RequestParser.hpp"

#include <map>
#include <string>
#include <vector>

struct BodyParsing
{
	ParseStatus status = ParseStatus::BAD;
	std::string body;
	std::size_t bytes = 0;
};

BodyParsing parse_body(
	const std::vector<uint8_t>& buffer,
	const Headers& headers,
	std::size_t body_start);
