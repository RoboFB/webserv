/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 13:33:13 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/25 13:51:26 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "MagicValues.hpp"

Location::Location(const LocationConfig &from)
	: error_codes(from.error_codes), indexs_paths(from.indexs_paths),
	  error_page_path(from.error_page_path.value_or("./example")),
	  root_path(from.root_path.value_or(".")), // todo
	  return_path(from.return_path.value_or("./example")),
	  client_max_body_size(
		  from.client_max_body_size.value_or(BODY_SIZE_DEFAULT)), // todo
	  return_code(from.return_code.value_or(888)), // todo
	  autoindex(from.autoindex.value_or(false)), limit_except(from.limit_except)
{
	if (error_codes.empty())
		error_codes.push_back(999); // todo
	if (indexs_paths.empty())
		indexs_paths.push_back("index.html"); // todo

	if (limit_except == Methods::NONE)
		limit_except = Methods::GET; // todo
}