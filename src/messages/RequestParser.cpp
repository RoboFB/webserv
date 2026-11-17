/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:51:03 by modiepge          #+#    #+#             */
/*   Updated: 2026/11/02 00:13:41 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <limits>

/*	Called again on every new chunk of data; the head is only parsed once,
	otherwise parse_headers would add every header a second time
 */
ParseStatus Request::request_parsing()
{
	if (!head_done_)
	{
		const ParseStatus head = parse_request_head();

		if (head != ParseStatus::COMPLETE)
			return head;
	}
	return parse_body(std::numeric_limits<std::size_t>::max());
}
