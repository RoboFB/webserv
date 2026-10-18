/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   URITarget.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 23:16:16 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 23:32:12 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

struct URITarget
{
	std::string path;
	std::string query;
};

URITarget split_uri_target(const std::string& target);
