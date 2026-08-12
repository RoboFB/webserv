/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 12:51:42 by modiepge          #+#    #+#             */
/*   Updated: 2026/08/12 18:32:27 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <map>
#include <vector>
#include <filesystem>

#include "Methods.hpp"


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



class Message
{
	protected:
		std::string version_ = "HTTP/1.0";
		std::map<std::string, std::vector<std::string>> headers_;
		std::string body_;
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
		Methods				method_;
		std::filesystem::path	target_; //path
	public:
		Request();
		Request(Methods method, const std::string &target);
		void 		setMethod(Methods method);
		Methods	getMethod() const;
		void 		setTarget(const std::string &target);
		std::filesystem::path getTarget() const;
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
		int status_ = 200;
	public:
		Response();
		Response(int status);
		void		setStatus(int status);
		int			getStatus() const;
		std::string	getReason() const;
		std::string	serialize() const;
};
