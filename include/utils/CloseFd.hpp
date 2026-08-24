/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CloseFd.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 14:04:52 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/24 16:11:14 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// wrapper for a file descriptor (fd)
// - implicitly converts to int, be carful
// - automatically closes the fd
class CloseFd
{
	public:
		CloseFd(const CloseFd &) = delete;
		CloseFd &operator=(const CloseFd &) = delete;

		CloseFd(CloseFd &&other);
		CloseFd &operator=(CloseFd &&other);

		CloseFd(int &&fd);
		~CloseFd();

		// be careful, but best tradeoff for easy use
		operator int() const;

	private:
		int raw_fd_;
		void better_close(int &fd);
};