/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:49:51 by modiepge          #+#    #+#             */
/*   Updated: 2026/09/21 01:29:17 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Message.hpp"
#include <algorithm>
#include <cctype>

enum class RequestStatus {
	MSG_INCOMPLETE,
	MSG_BAD,
	MSG_COMPLETE
};

struct RequestParsing
{
	RequestStatus status = RequestStatus::MSG_BAD;
	Request message;
	std::size_t	bytes = 0;
};

RequestParsing request_parsing(const std::vector<uint8_t>& buffer);