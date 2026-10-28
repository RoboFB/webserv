/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 12:51:42 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/28 15:42:59 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Message.hpp"
#include "Response.hpp"

#include "Methods.hpp"
#include "Server.hpp"

struct URITarget
{
		std::string path;
		std::string query;
};

enum class ParseStatus
{
	INCOMPLETE,
	BAD,
	COMPLETE
};

class Request : public Message
{
	private:
		Methods method_;
		std::string target_; // this needs to be a string because it is a URI,
							 // so that we may interpret things like ?key=value
		bool head_done_ = false;

		bool is_version_valid() const;
		bool is_target_valid() const;
		bool is_method_valid() const;

	public:
		Request();
		Request(Methods method, const std::string &target);
		std::string getTarget() const;
		void serialize() override;

		// ParseStatus Request::parse_request_head()

		ParseStatus parse_request_head();
		ParseStatus parse_body();
		ParseStatus request_parsing();

		URITarget split_uri_target() const;

		bool parse_request_line(const std::string &line);
		bool parse_headers(const std::string &block, std::size_t cursor);
		bool has_required_host() const;

		Methods get_method() const
		{
			return method_;
		}
};

void handle_request(const Request &request, Response &response,
					const Server &server);
void handle_get(Response &response, const Location &location,
				const std::string &request_path);
void handle_post(const std::string &data_str_body, Response &response,
				 const Location &location, const std::string &request_path);
void handle_delete(Response &response, const Location &location,
				   const std::string &request_path);