/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 12:51:42 by modiepge          #+#    #+#             */
/*   Updated: 2026/11/01 21:48:53 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Message.hpp"

class Location;

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

		std::string head_;
		std::size_t head_offset_ = 0;
		std::size_t body_offset_ = 0;

	public:
		Response();
		std::string getReason() const;
		void serialize() override;

		void set_status(int status);
		void convert_to_error(int status);
		void convert_to_not_allowed(const Location &location);
		void convert_to_redirect(int status, const std::string &location);

		Response &addHeader(const std::string &name,
							const std::string &value);

		const char *send_data() const;
		std::size_t send_size() const;
		void consume_sent(std::size_t bytes);
		bool is_fully_sent() const;
};

Response make_error_response(int status);
