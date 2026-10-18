/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:49:51 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 21:29:37 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Message.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

enum class ParseStatus {
	INCOMPLETE,
	BAD,
	COMPLETE
};

struct RequestParsing
{
	ParseStatus status = ParseStatus::BAD;
	Request message;
	std::size_t bytes = 0;
};

RequestParsing request_parsing(
	const std::vector<uint8_t>& buffer);
