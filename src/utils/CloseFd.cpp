/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CloseFd.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 14:09:37 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/24 16:15:22 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CloseFd.hpp"
#include "logging.hpp"

#include <unistd.h>
#include <cstring>

CloseFd::CloseFd(int &&fd) : raw_fd_(fd) {}

void CloseFd::better_close(int &fd)
{
	if (fd >= 0)
	{
		if (::close(fd) == -1)
			LOG(LOG_ERROR,
				"close fd class: " + std::string(std::strerror(errno)));
		fd = -1;
	}
}

CloseFd::~CloseFd()
{
	better_close(raw_fd_);
}

// move a fd
CloseFd::CloseFd(CloseFd &&other) : raw_fd_(other.raw_fd_)
{
	other.raw_fd_ = -1;
}

CloseFd &CloseFd::operator=(CloseFd &&other)
{
	if (this != &other)
	{
		better_close(raw_fd_);
		raw_fd_ = other.raw_fd_;
		other.raw_fd_ = -1;
	}
	return *this;
}

CloseFd::operator int() const
{
	return raw_fd_;
}
