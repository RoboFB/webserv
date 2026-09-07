/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 00:00:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/09/07 15:39:56 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollHandler.hpp"
#include "logging.hpp"
#include <cstring>
#include <sys/epoll.h>

EpollHandler::EpollHandler(const Server *server, CloseFd &&fd,
						   const CloseFd &epoll_fd)
	: server_(server), epoll_fd_(epoll_fd), fd_(std::move(fd)),
	  remove_me_(false)
{
}

// set it with EPOLL_CTL_ADD and EPOLLIN
void EpollHandler::add_to_epoll()
{
	struct epoll_event event;
	event.events = EPOLL_EVENTS::EPOLLIN;
	event.data.ptr = this;

	if (::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd_, &event) < 0)
	{
		LOG(LOG_ERROR, "epoll_ctl add: " + std::string(std::strerror(errno)));
	}
}

// switch this fd's registered interest (e.g. EPOLLIN -> EPOLLOUT) once
// already added; use add_to_epoll() for the initial registration instead.
void EpollHandler::modify_epoll(uint32_t events)
{
	struct epoll_event event;
	event.events = events;
	event.data.ptr = this;

	if (::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd_, &event) < 0)
	{
		LOG(LOG_ERROR, "epoll_ctl mod: " + std::string(std::strerror(errno)));
	}
}

void EpollHandler::remove_from_epoll() const
{
	if (::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd_, nullptr) < 0)
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