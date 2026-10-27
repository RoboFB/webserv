/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:45:38 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/27 16:43:53 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

// Cgi::Cgi(const Server *server, CloseFd &&fd, const CloseFd &epoll_fd)
// 	: EpollHandler(server, std::move(fd), epoll_fd)
// {
// }

void Cgi::on_epoll_event(AllServers &servers, uint32_t events)
{
	(void)servers;
	(void)events;
}
bool Cgi::is_timeout() const
{
	return false;
}

void Cgi::stop_listening(void) {}