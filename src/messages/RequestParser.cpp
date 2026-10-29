/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:51:03 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/27 16:29:45 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

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
		head_done_ = true;
	}
	return parse_body();
}
