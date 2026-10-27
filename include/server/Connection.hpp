/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/27 18:23:01 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Cgi.hpp"
#include "CloseFd.hpp"
#include "EpollHandler.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <sys/types.h>

#include <vector>
#include <cstdint>
#include <chrono>

class Server;

// wraps an already-accepted client fd (from SocketFd::accept()).
// unlike SocketFd, a Connection never listens/accepts, it only reads/writes
// on a fd that already refers to a connected client.
class Connection : public EpollHandler
{
	public:
		Connection(const Connection &) = delete;
		Connection &operator=(const Connection &) = delete;

		Connection(Connection &&) = delete;
		Connection &operator=(Connection &&) = delete;

		Connection(const Server *server, CloseFd &&fd, const CloseFd &epoll_fd);
		~Connection() override = default;

		void on_epoll_event(AllServers &servers, uint32_t events) override;
		bool is_timeout() const override;
		void stop_listening() override;

	private:
		void receive();
		void send();
		void queue_response();
		void finish();

		// std::vector<uint8_t> request_buffer_;
		Request request_;
		std::optional<Cgi> cgi_;
		Response response_;
		// std::vector<uint8_t> response_buffer_;
		// std::size_t response_offset_ = 0;
		enum class State
		{
			RECEIVING,
			// RECEIVING_FINISHED,
			// BUILTING,
			// BUILT_FINISHED,
			SENDING,
			// SENDING_FINISHED,
			FINISHED
		} state_;

		std::chrono::steady_clock::time_point last_activity_;
};
