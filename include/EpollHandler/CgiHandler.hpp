/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:46:20 by rgohrig           #+#    #+#             */
/*   Updated: 2026/11/02 12:33:30 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <filesystem>
#include <string>
#include <chrono>
#include <cstddef>
#include <sys/types.h>

#include "EpollHandler.hpp"
#include "Request.hpp"

class ConnectionHandler;
class Response;

class CgiHandler : public EpollHandler
{
	public:
		CgiHandler(const CgiHandler &) = delete;
		CgiHandler &operator=(const CgiHandler &) = delete;

		CgiHandler(CgiHandler &&) = delete;
		CgiHandler &operator=(CgiHandler &&) = delete;

		CgiHandler(const Server *server, const CloseFd &epoll_fd,
				ConnectionHandler &connection, Response &response,
				const Request &cgi_request,
				const std::filesystem::path &script_path,
				const std::string &interpreter_path);

		~CgiHandler() override = default;

		void on_epoll_event(AllServers &servers, uint32_t events) override;
		bool is_timeout() const override;
		bool is_started() const;
		void stop_listening(void) override;

	private:
		void write_request_body();
		void read_cgi_output();
		void finish_cgi_response();
		void close_cgi_input();
		void fail_cgi();

		ConnectionHandler &connection_;
		Response &response_;

		const std::string &request_body_;
		std::size_t request_body_offset_;
		std::string cgi_output_;

		CloseFd input_fd_;

		pid_t child_pid_;
		bool child_input_closed_;
		std::chrono::steady_clock::time_point last_activity_;
};
