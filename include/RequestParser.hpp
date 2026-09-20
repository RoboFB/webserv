/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:49:51 by modiepge          #+#    #+#             */
/*   Updated: 2026/09/20 18:50:40 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

enum class RequestStatus {
	MSG_INCOMPLETE,
	MSG_BAD,
	MSG_COMPLETE
};

struct RequestParsing
{
	RequestStatus status = RequestStatus::MSG_BAD;
	Request message;
	std::size_t	bytes;
};
