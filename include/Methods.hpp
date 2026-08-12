/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 18:28:34 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/12 18:35:57 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

enum class Methods
{
	NONE =    0b00000, // "NONE" or if not specified
	GET =    0b00001, // "GET"
	POST =   0b00010, // "POST"
	DELETE = 0b00100  // "DELETE"	
};

inline Methods operator|(Methods a, Methods b) {
	return static_cast<Methods>(
		static_cast<int>(a) | static_cast<int>(b)
    );
}

// does not work with NONE, because NONE is 0, so it will always return false
inline bool is_method(Methods combined_methods, Methods one_method) {
	return (static_cast<int>(combined_methods) & static_cast<int>(one_method)) != 0;
}

// no throw, if not found return NONE
Methods string_to_methods(const std::string& methods);

std::string methods_to_string(Methods methods);

