/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:04:05 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 21:39:46 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Methods.hpp"

Methods string_to_methods(const std::string& string) {
	if (string == "GET")
		return (Methods::GET);
	else if (string == "POST")
		return (Methods::POST);
	else if (string == "DELETE")
		return (Methods::DELETE);
	return (Methods::NONE);
}

std::string methods_to_string(Methods method)
{
	if (method == Methods::GET)
		return "GET";
	if (method == Methods::POST)
		return "POST";
	if (method == Methods::DELETE)
		return "DELETE";
	return "";
}
