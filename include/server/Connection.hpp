/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 20:15:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/21 01:21:55 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CloseFd.hpp"
#include "EpollHandler.hpp"
#include "Message.hpp"
#include "RequestHandlers.hpp"
#include <string>
#include <sys/types.h>

#include <vector>
#include <cstdint>

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
		~Connection() override;

		void on_epoll_event(AllServers &servers, uint32_t events) override;

	private:
		void receive(void);
		void make_response(const Request& request);
		void send(void);

		std::vector<uint8_t> request_buffer_;
		std::vector<uint8_t> response_buffer_;
		std::size_t response_offset_ = 0;
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
};
