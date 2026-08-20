/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFds.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 20:21:16 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/20 21:29:22 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AddrInfoPtr.hpp"
#include "SocketFd.hpp"
#include <vector>

class SocketFds
{
	public:
		SocketFds(const SocketFds &) = delete;
		SocketFds &operator=(const SocketFds &) = delete;
		
		SocketFds(SocketFds &&) = default;
		SocketFds &operator=(SocketFds &&) = delete;
		
		
		SocketFds(const AddrInfoPtr &start);
		

		void listen(void);

		std::vector<int> accept(void);

		SocketFd &at(size_t index);

	private:
		std::vector<SocketFd> socket_fds_;
};

