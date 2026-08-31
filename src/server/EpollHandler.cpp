/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 00:00:00 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/31 20:07:10 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollHandler.hpp"
#include "logging.hpp"
#include <cstring>
#include <sys/epoll.h>

// void EpollHandler::accept_connection(SocketFd &listener)
// {
// 	int new_fd = listener.accept();

// 	auto insert_result = accepted_fds_.emplace(
// 		new_fd, Connection(listener.server(), CloseFd(std::move(new_fd))));
// 	Connection &connection = insert_result.first->second;

// 	connection.add_to_epoll(epoll_fd_);
// 	connection.send(response2); // todo: for testing only, remove later
// }

// void EpollHandler::close_connection(int fd)
// {
// 	accepted_fds_.erase(fd);
// }

EpollHandler::EpollHandler(const Server *server, CloseFd &&fd)
	: server_(server), fd_(std::move(fd))
{
}

void EpollHandler::add_to_epoll(const CloseFd &epoll_fd) const
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = const_cast<EpollHandler *>(this);

	if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_, &ev) < 0)
	{
		LOG(LOG_ERROR, "epoll_ctl: " + std::string(std::strerror(errno)));
	}
	LOG(LOG_DEBUG, "Added client fd " + std::to_string(fd_) + " to epoll_fd_ " +
					   std::to_string(epoll_fd));
}