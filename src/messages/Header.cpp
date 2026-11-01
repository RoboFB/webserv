/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/01 17:40:08 by modiepge          #+#    #+#             */
/*   Updated: 2026/11/01 21:15:59 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"

#include <cctype>
#include <utility>

static std::string normalize_header_name(std::string name)
{
	for (char &character : name)
	{
		character = static_cast<char>(
			std::tolower(static_cast<unsigned char>(character)));
	}
	return name;
}

void Header::add(std::string name, std::string value)
{
	name = normalize_header_name(std::move(name));
	fields_[name].push_back(std::move(value));
}

void Header::set(std::string name, std::string value)
{
	name = normalize_header_name(std::move(name));
	fields_[name] = HeaderValues{std::move(value)};
}

bool Header::remove(const std::string &name)
{
	return fields_.erase(normalize_header_name(name)) != 0;
}

const HeaderValues *Header::find(const std::string &name) const
{
	const HeaderMap::const_iterator found =
		fields_.find(normalize_header_name(name));

	if (found == fields_.end())
		return nullptr;
	return &found->second;
}

bool Header::contains(const std::string &name) const
{
	return find(name) != nullptr;
}

void Header::serialize(std::string &out) const
{
	for (const auto &field : fields_)
	{
		for (const std::string &value : field.second)
		{
			out += field.first;
			out += ": ";
			out += value;
			out += LINE;
		}
	}
}

void Header::clear()
{
	fields_.clear();
}
