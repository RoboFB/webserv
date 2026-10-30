/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:46:20 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/28 16:59:57 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "EpollHandler.hpp"
#include "Request.hpp"

class CgiHandler : public EpollHandler
{
	public:
		CgiHandler(const CgiHandler &) = delete;
		CgiHandler &operator=(const CgiHandler &) = delete;

		CgiHandler(CgiHandler &&) = delete;
		CgiHandler &operator=(CgiHandler &&) = delete;

		CgiHandler(const Server *server, const CloseFd &epoll_fd,
				   const Request &cgi_request);

		~CgiHandler() override = default;

		void on_epoll_event(AllServers &servers, uint32_t events) override;
		bool is_timeout() const override;
		void stop_listening(void) override;

	private:
		// std::chrono::steady_clock::time_point last_activity_;
};
