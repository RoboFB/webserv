/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 13:27:16 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "EpollHandler.hpp"
#include "CgiHandler.hpp"
#include "CloseFd.hpp"

#include "Response.hpp"
#include "Request.hpp"

#include <sys/types.h>
#include <cstdint>
#include <chrono>

class Server;

// wraps an already-accepted client fd (from SocketFd::accept()).
// unlike SocketFd, a Connection never listens/accepts, it only reads/writes
// on a fd that already refers to a connected client.
class ConnectionHandler : public EpollHandler
{
	public:
		ConnectionHandler(const ConnectionHandler &) = delete;
		ConnectionHandler &operator=(const ConnectionHandler &) = delete;

		ConnectionHandler(ConnectionHandler &&) = delete;
		ConnectionHandler &operator=(ConnectionHandler &&) = delete;

		ConnectionHandler(const Server *server, CloseFd &&fd,
						  const CloseFd &epoll_fd);
		~ConnectionHandler() override = default;

		void on_epoll_event(AllServers &servers, uint32_t events) override;
		bool is_timeout() const override;
		void stop_listening() override;

	private:
		void receive();
		void make_response();
		void send();
		void queue_response();
		void finish();

		Request request_;
		const CgiHandler *cgi_;
		Response response_;

		enum State
		{
			READ_HEAD,
			READ_BODY,
			WAIT_CGI,
			WRITE,
			FINISHED

		} state_;
		std::chrono::steady_clock::time_point last_activity_;
		void set_state(State new_state);
};
