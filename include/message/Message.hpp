/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 12:51:42 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/22 13:25:09 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <vector>

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

#define LINE "\r\n"

using Headers = std::map<std::string, std::vector<std::string>>;

class Message
{
	protected:
		std::string version_;
		Headers headers_;
		std::string body_;
		std::string serializeHeaders() const;

	public:
		Message(const std::string &version = "HTTP/1.0");
		virtual ~Message();
		Message &addHeader(const std::string &name, const std::string &value);
		const Headers &getHeaders() const;
		void setBody(const std::string &body);
		const std::string &getBody() const;
		virtual std::string serialize() const = 0;
};

int hex_value(char character);
bool percent_decode(const std::string &input, std::string &output);