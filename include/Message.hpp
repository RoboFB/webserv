/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 12:51:42 by modiepge          #+#    #+#             */
/*   Updated: 2026/08/12 18:11:03 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <map>
#include <vector>
#include <filesystem>

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

enum	method_e {
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE,
	PATCH
};

class Message
{
	protected:
		std::string version = "HTTP/1.0";
		std::map<std::string, std::vector<std::string>> headers;
		std::string body;
		std::string serializeHeaders() const;
	public:
		Message(/* args */);
		virtual ~Message();
		void setVersion(const std::string &version);
		const std::string &getVersion() const;
		void addHeader(const std::string &name, const std::string &value);
		const std::map<std::string, std::vector<std::string>> &getHeaders() const;
		void setBody(const std::string &body);
		const std::string &getBody() const;
		virtual std::string serialize() const = 0;
};

class Request : public Message
{
	private:
		method_e				method;
		std::filesystem::path	target; //path
	public:
		Request();
		Request(method_e method, const std::string &target);
		void 		setMethod(method_e method);
		method_e	getMethod() const;
		void 		setTarget(const std::string &target);
		const std::string &getTarget() const;
		std::string serialize() const;
};

class Response : public Message
{
	private:
		inline static	const std::map<int, std::string> reasons = {{200, "OK"},
			{201, "Created"}, {204, "No Content"}, {301, "Moved Permanently"},
			{302, "Found"}, {400, "Bad Request"}, {403, "Forbidden"},
			{404, "Not Found"}, {405, "Method Not Allowed"},{408, "Request Timeout"},
			{413, "Payload Too Large"}, {414, "URI Too Long"},
			{415, "Unsupported Media Type"}, {500, "Internal Server Error"},
			{501, "Not Implemented"}, {502, "Bad Gateway"},
			{503, "Service Unavailable"}, {504, "Gateway Timeout"},
			{505, "HTTP Version Not Supported"}};
		int status = 200;
	public:
		Response();
		Response(int status);
		void		setStatus(int status);
		int			getStatus() const;
		std::string	getReason() const;
		std::string	serialize() const;
};
