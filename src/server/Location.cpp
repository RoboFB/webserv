/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 13:33:13 by rgohrig           #+#    #+#             */
/*   Updated: 2026/11/02 00:39:09 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "MagicValues.hpp"

#include "ConfigParserStructs.hpp"

Location::Location(const LocationConfigParser &from)
	: location_path(from.location_path), indexs_paths(from.indexs_paths),
	  cgi_interpers(from.cgi_interpers), error_pages(from.error_pages),
	  redirect_page(from.redirect_page),
	  root_path(), upload_store(from.upload_store), client_max_body_size(
					   from.client_max_body_size.value_or(BODY_SIZE_DEFAULT)),
	  autoindex(from.autoindex.value_or(false)), limit_except(from.limit_except)
{
	if (from.root_path == std::nullopt && !redirect_page)
	{
		throw std::runtime_error("root path is not set for location: " +
								from.location_path);
	}

	if (from.root_path)
		root_path = from.root_path.value();

	if (indexs_paths.empty())
		indexs_paths.push_back("index.html");

	if (limit_except == Methods::NONE)
		limit_except = Methods::GET;
}