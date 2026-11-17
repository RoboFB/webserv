/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:04:05 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/20 15:41:46 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Methods.hpp"

Methods string_to_methods(const std::string &string)
{
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
