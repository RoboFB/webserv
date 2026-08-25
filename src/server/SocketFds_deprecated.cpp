/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFds.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 20:21:12 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/25 14:22:37 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "SocketFds.hpp"

// SocketFds::SocketFds(const AddrInfoPtr &start) : socket_fds_()
// {
// 	for (const struct addrinfo *current = start.get(); current != nullptr;
// 		 current = current->ai_next)
// 	{
// 		socket_fds_.push_back(SocketFd(current));
// 	}
// }

// void SocketFds::listen()
// {
// 	for (auto &socket_fd : socket_fds_)
// 	{
// 		socket_fd.listen();
// 	}
// }

// std::vector<int> SocketFds::accept()
// {
// 	std::vector<int> new_fds;
// 	for (SocketFd &socket_fd : socket_fds_)
// 	{
// 		new_fds.push_back(socket_fd.accept());
// 	}
// 	return new_fds;
// }

// void SocketFds::add_to_epoll(const CloseFd &epoll_fd, void *server)
// {
// 	for (SocketFd &socket_fd : socket_fds_)
// 	{
// 		socket_fd.add_to_epoll(epoll_fd, server);
// 	}
// }

// SocketFd &SocketFds::at(size_t index)
// {
// 	return socket_fds_.at(index);
// }