/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 00:00:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/29 00:00:00 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class AllServers;

// common interface for anything registered with epoll via data.ptr
// (SocketFd for listeners, Connection for accepted clients). lets
// AllServers::wait_epoll dispatch straight to the object that owns the
// ready fd, instead of searching listen_fds_/accepted_fds_ for a match.
class EpollHandler
{
	public:
		virtual ~EpollHandler() = default;
		virtual void on_epoll_event(AllServers &servers) = 0;
};
