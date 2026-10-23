/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserBody.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 20:54:35 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/20 21:23:49 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "RequestParser.hpp"

#include <string>
#include <vector>

// default:  status: BAD, body: empty, bytes: 0
struct BodyParsing
{
		ParseStatus status = ParseStatus::BAD;
		std::string body;
		std::size_t bytes = 0;

		BodyParsing(ParseStatus status = ParseStatus::BAD,
					const std::string &body = "", std::size_t bytes = 0)
			: status(status), body(body), bytes(bytes)
		{
		}
};

BodyParsing parse_body(const std::vector<uint8_t> &buffer,
					   const Headers &headers, std::size_t body_start);
