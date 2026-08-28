/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 15:57:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 19:29:52 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Location.hpp"

#include "AddrInfoPtr.hpp"
#include "ConfigStructs.hpp"
#include "SocketFd.hpp"

#include <vector>

class Server
{
	public:
		Server(Server &&) = default;
		Server(ServerConfig &server_config, std::vector<SocketFd> &all_fds);

		Server(const Server &) = delete;
		Server &operator=(const Server &) = delete;
		Server &operator=(Server &&) = delete;

	private:
		std::vector<Location> locations_;
		AddrInfoPtr addr_listen_list_; // linked list unique pointer to head,
									   // mostly for setup
};
