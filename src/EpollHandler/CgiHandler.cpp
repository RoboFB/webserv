/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:45:38 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/29 13:20:56 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "logging.hpp"

#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

CgiHandler::CgiHandler(const Server *server, const CloseFd &epoll_fd,
					   const Request &cgi_request)
	: EpollHandler(server, -1, epoll_fd)
{
	(void)cgi_request;

	int both_fds[2];
	if (::socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0,
					 both_fds) < 0)
	{
		LOG(LOG_ERROR, "socketpair: " + std::string(std::strerror(errno)));
		return;
	}
	pid_t pid = fork();
	if (pid < 0)
	{
		close(both_fds[0]);
		close(both_fds[1]);
		LOG(LOG_ERROR, "fork: " + std::string(std::strerror(errno)));
		return;
	}
	else if (pid == 0)
	{
		// child process
		close(both_fds[0]);
		dup2(both_fds[1], STDIN_FILENO);
		dup2(both_fds[1], STDOUT_FILENO);
		close(both_fds[1]);
		char *argv[] = {const_cast<char *>("/usr/bin/python3"),
						const_cast<char *>("todo"), nullptr};
		char *envp[] = {const_cast<char *>("GATEWAY_INTERFACE=CGI/1.1"),
						const_cast<char *>("SERVER_PROTOCOL=HTTP/1.1"),
						const_cast<char *>("REQUEST_METHOD=GET"),
						const_cast<char *>("QUERY_STRING=a=1"),
						const_cast<char *>("CONTENT_LENGTH=0"),
						nullptr};
		execve(argv[0], argv, envp);
		exit(99); // todo: cleanup and exit
	}
	else
	{
		// parent process
		close(both_fds[1]);
		fd_ = int(both_fds[0]);
		this->add_to_epoll();
	}
}

void CgiHandler::on_epoll_event(AllServers &servers, uint32_t events)
{
	if (events & EPOLL_EVENTS::EPOLLIN)
	{
		// read from cgi and send to client
		//
		modify_epoll(EPOLL_EVENTS::EPOLLOUT);
	}
	if (events & EPOLL_EVENTS::EPOLLOUT)
	{
	}

	(void)servers;
	(void)events;
}

bool CgiHandler::is_timeout() const
{
	return false;
}

void CgiHandler::stop_listening(void)
{
	if (remove_me_)
		return;
	remove_from_epoll();
	remove_me_ = true;
}