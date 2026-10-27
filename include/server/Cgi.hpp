/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:46:20 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/27 14:51:57 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "EpollHandler.hpp"

class Cgi : public EpollHandler
{
	public:
		Cgi(const Cgi &) = delete;
		Cgi &operator=(const Cgi &) = delete;

		Cgi(Cgi &&) = delete;
		Cgi &operator=(Cgi &&) = delete;

		Cgi(const Server *server, CloseFd &&fd, const CloseFd &epoll_fd);
		~Cgi() override = default;

		void on_epoll_event(AllServers &servers, uint32_t events) override;
		bool is_timeout() const override;
		void stop_listening(void) override;

	private:
		CloseFd cgi_connection_fd_;

		// std::chrono::steady_clock::time_point last_activity_;
};
