/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 00:00:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/01 16:56:55 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollHandler.hpp"
#include "logging.hpp"
#include <cstring>
#include <sys/epoll.h>

EpollHandler::EpollHandler(const Server *server, CloseFd &&fd)
	: server_(server), fd_(std::move(fd)), remove_me_(false)
{
}

void EpollHandler::add_to_epoll(const CloseFd &epoll_fd) const
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = const_cast<EpollHandler *>(this);

	if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_, &ev) < 0)
	{
		LOG(LOG_ERROR, "epoll_ctl add: " + std::string(std::strerror(errno)));
	}
}

void EpollHandler::remove_from_epoll(const CloseFd &epoll_fd) const
{
	if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd_, nullptr) < 0)
	{
		LOG(LOG_ERROR,
			"epoll_ctl remove: " + std::string(std::strerror(errno)));
	}
}

void EpollHandler::set_remove_me(void)
{
	remove_me_ = true;
}

bool EpollHandler::is_remove_me(void) const
{
	return remove_me_;
}