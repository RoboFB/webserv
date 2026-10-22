/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 12:51:42 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/21 13:56:55 by rgohrig          ###   ########.fr       */
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
