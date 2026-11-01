/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/01 17:31:08 by modiepge          #+#    #+#             */
/*   Updated: 2026/11/01 20:26:13 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>

using HeaderValues = std::vector<std::string>;
using HeaderMap = std::map<std::string, HeaderValues>;

#define LINE "\r\n"

class Header
{
	private:
		HeaderMap fields_;

	public:
		void				add(std::string name, std::string value);
		void				set(std::string name, std::string value);
		bool				remove(const std::string &name);

		const HeaderValues	*find(const std::string &name) const;
		bool				contains(const std::string &name) const;

		void				serialize(std::string &out) const;
		// void				**http_header_to_envp(const char **envp);

		void				clear();
};
