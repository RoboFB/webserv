/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   URITarget.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 23:17:05 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 23:32:10 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "URITarget.hpp"

URITarget split_uri_target(const std::string& target)
{
	const std::size_t query_start = target.find('?');

	if (query_start == std::string::npos)
		return URITarget{target, ""};

	return URITarget{
		target.substr(0, query_start),
		target.substr(query_start + 1)
	};
}
