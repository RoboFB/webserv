/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParserHead.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 21:29:05 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 21:29:30 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "RequestParser.hpp"

struct HeaderParsing
{
	ParseStatus status = ParseStatus::BAD;
	Request request;
	std::size_t body_start = 0;
};

HeaderParsing parse_request_head(const std::vector<uint8_t>& buffer);
