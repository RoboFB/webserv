/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:49:51 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/20 21:44:38 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Message.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

enum class ParseStatus
{
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

RequestParsing request_parsing(const std::vector<uint8_t> &buffer);
