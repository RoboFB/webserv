/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 12:51:42 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/20 19:43:45 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <filesystem>

#include "Methods.hpp"
#include "Server.hpp"

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

struct URITarget
{
		std::string path;
		std::string query;
};

class Message
{
	protected:
		std::string version_ = "HTTP/1.0";
		Headers headers_;
		std::string body_;
		std::string serializeHeaders() const;

	public:
		Message(/* args */);
		virtual ~Message();
		const std::string &getVersion() const;
		Message &setVersion(const std::string &version);
		Message &addHeader(const std::string &name, const std::string &value);
		const Headers &getHeaders() const;
		void setBody(const std::string &body);
		const std::string &getBody() const;
		virtual std::string serialize() const = 0;
};

class Response;

class Request : public Message
{
	private:
		Methods method_;
		std::string target_; // this needs to be a string because it is a URI,
							 // so that we may interpret things like ?key=value

		bool is_version_valid() const;
		bool is_target_valid() const;
		bool is_method_valid() const;

	public:
		Request();
		Request(Methods method, const std::string &target);
		std::string getTarget() const;
		std::string serialize() const;

		URITarget split_uri_target() const;
		Response handle_request(const Server &server) const;
		Response handle_get(const Location &location,
							const std::string &request_path) const;
		Response handle_post(const Location &location,
							 const std::string &request_path) const;
		Response handle_delete(const Location &location,
							   const std::string &request_path) const;

		bool parse_request_line(const std::string &line);
		bool parse_headers(const std::string &block, std::size_t cursor);
		bool has_required_host() const;
};

class Response : public Message
{
	private:
		inline static const std::map<int, std::string> reasons_ = {
			{200, "OK"},
			{201, "Created"},
			{204, "No Content"},
			{301, "Moved Permanently"},
			{302, "Found"},
			{400, "Bad Request"},
			{403, "Forbidden"},
			{404, "Not Found"},
			{405, "Method Not Allowed"},
			{408, "Request Timeout"},
			{413, "Payload Too Large"},
			{414, "URI Too Long"},
			{415, "Unsupported Media Type"},
			{500, "Internal Server Error"},
			{501, "Not Implemented"},
			{502, "Bad Gateway"},
			{503, "Service Unavailable"},
			{504, "Gateway Timeout"},
			{505, "HTTP Version Not Supported"}};
		int status_ = 200;
		void setStatus(int status);

	public:
		Response();
		Response(int status);
		std::string getReason() const;
		std::string serialize() const;

		Response &addHeader(const std::string &name, const std::string &value);
};

int hex_value(char character);
bool percent_decode(const std::string &input, std::string &output);
Response make_error_response(int status);
