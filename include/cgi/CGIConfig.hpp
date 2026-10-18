/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIConfig.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 23:37:36 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/19 00:40:21 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <cstddef>
#include <map>
#include <filesystem>

/*	this should take e.g. <.php, /path/to/php> or <.py, /usr/bin/python3> plus
	timeout since we have to do this non-blocking.
*/
struct CGIConfig
{
	std::map<std::string, std::filesystem::path> interpreter_by_extension;
	std::size_t timeout_ms = 5000;
};
