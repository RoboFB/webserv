/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:51:03 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/22 13:34:46 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include "RequestParserBody.hpp"
#include "RequestParserHead.hpp"

RequestParsing request_parsing(const std::vector<uint8_t> &buffer)
{
	RequestParsing result;
	const HeaderParsing head = parse_request_head(buffer);
	if (head.status != ParseStatus::COMPLETE)
	{
		result.status = head.status;
		return result;
	}

	const BodyParsing body =
		parse_body(buffer, head.request.getHeaders(), head.body_start);
	if (body.status != ParseStatus::COMPLETE)
	{
		result.status = body.status;
		return result;
	}

	result.message = head.request;
	result.message.setBody(body.body);
	result.bytes = body.bytes;
	result.status = ParseStatus::COMPLETE;
	return result;
}
