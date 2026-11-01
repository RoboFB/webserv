/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 12:51:42 by modiepge          #+#    #+#             */
/*   Updated: 2026/11/02 00:38:58 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>

#include "Header.hpp"

/*	required:
	- response header (create)
	- request header (parse)

	response components:
	- version
	- response code
	- headers (maybe hardcode?)
	- body

	request components:
	- method
	- path
	- headers
	- body

	shared components:
	- headers
	- body

	function:
	make parametric message constructors parse a request/generate a response
	Request(request) -> parse and do stuff
*/

class Message
{
	protected:
		std::string version_;
		Header headers_;
		std::string body_;
		std::string serializeHeaders() const;

	public:
		std::size_t offset_;
		std::vector<uint8_t> buffer_;

		Message(const std::string &version = "HTTP/1.0");
		virtual ~Message();
		Message &addHeader(const std::string &name, const std::string &value);
		Message &setBody(std::string body);
		const Header &get_headers() const;
		const std::string &getBody() const;
		virtual void serialize() = 0;
};

int hex_value(char character);
std::string percent_encode(const std::string &input);
bool percent_decode(const std::string &input, std::string &output);
